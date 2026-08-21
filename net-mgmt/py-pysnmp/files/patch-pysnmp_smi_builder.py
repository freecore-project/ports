--- pysnmp/smi/builder.py.orig
+++ pysnmp/smi/builder.py
@@ -6,13 +6,30 @@
 #
 import os
 import sys
-import imp
 import struct
 import marshal
 import time
 import traceback
 
 try:
+    from importlib import machinery, util
+
+    PY_MAGIC_NUMBER = util.MAGIC_NUMBER
+    SOURCE_SUFFIXES = machinery.SOURCE_SUFFIXES
+    BYTECODE_SUFFIXES = machinery.BYTECODE_SUFFIXES
+
+except ImportError:
+    import imp
+
+    PY_MAGIC_NUMBER = imp.get_magic()
+    SOURCE_SUFFIXES = [s[0] for s in imp.get_suffixes()
+                       if s[2] == imp.PY_SOURCE]
+    BYTECODE_SUFFIXES = [s[0] for s in imp.get_suffixes()
+                         if s[2] == imp.PY_COMPILED]
+
+PY_SUFFIXES = SOURCE_SUFFIXES + BYTECODE_SUFFIXES
+
+try:
     from errno import ENOENT
 except ImportError:
     ENOENT = -1
@@ -31,27 +48,16 @@
 class __AbstractMibSource(object):
     def __init__(self, srcName):
         self._srcName = srcName
-        self.__magic = imp.get_magic()
-        self.__sfx = {}
         self.__inited = None
-        for sfx, mode, typ in imp.get_suffixes():
-            if typ not in self.__sfx:
-                self.__sfx[typ] = []
-            self.__sfx[typ].append((sfx, len(sfx), mode))
         debug.logger & debug.flagBld and debug.logger('trying %s' % self)
 
     def __repr__(self):
         return '%s(%r)' % (self.__class__.__name__, self._srcName)
 
     def _uniqNames(self, files):
-        u = set()
-        for f in files:
-            if f.startswith('__init__.'):
-                continue
-            for typ in (imp.PY_SOURCE, imp.PY_COMPILED):
-                for sfx, sfxLen, mode in self.__sfx[typ]:
-                    if f[-sfxLen:] == sfx:
-                        u.add(f[:-sfxLen])
+        u = set(f[:-len(sfx)] for f in files
+                if not f.startswith('__init__.')
+                for sfx in PY_SUFFIXES if f.endswith(sfx))
         return tuple(u)
 
     # MibSource API follows
@@ -76,9 +82,9 @@
     def read(self, f):
         pycTime = pyTime = -1
 
-        for pycSfx, pycSfxLen, pycMode in self.__sfx[imp.PY_COMPILED]:
+        for pycSfx in BYTECODE_SUFFIXES:
             try:
-                pycData, pycPath = self._getData(f + pycSfx, pycMode)
+                pycData, pycPath = self._getData(f + pycSfx, 'rb')
 
             except IOError:
                 why = sys.exc_info()[1]
@@ -91,7 +97,7 @@
                     raise error.MibLoadError('MIB file %s access error: %s' % (f + pycSfx, why))
 
             else:
-                if self.__magic == pycData[:4]:
+                if PY_MAGIC_NUMBER == pycData[:4]:
                     pycData = pycData[4:]
                     pycTime = struct.unpack('<L', pycData[:4])[0]
                     pycData = pycData[4:]
@@ -103,7 +109,7 @@
                 else:
                     debug.logger & debug.flagBld and debug.logger('bad magic in %s' % pycPath)
 
-        for pySfx, pySfxLen, pyMode in self.__sfx[imp.PY_SOURCE]:
+        for pySfx in SOURCE_SUFFIXES:
             try:
                 pyTime = self._getTimestamp(f + pySfx)
 
@@ -125,7 +131,7 @@
             return marshal.loads(pycData), pycSfx
 
         if pyTime != -1:
-            modData, pyPath = self._getData(f + pySfx, pyMode)
+            modData, pyPath = self._getData(f + pySfx, 'r')
             return compile(modData, pyPath, 'exec'), pyPath
 
         raise IOError(ENOENT, 'No suitable module found', f)
