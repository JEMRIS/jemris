# -*- mode: python -*-

block_cipher = None

a = Analysis(['jemris.py'],
             binaries=None,
             datas=[('gui/ui/*.ui',          'gui/ui'),
    		         ('gui/settings/*.dat',   'gui/settings'),
                    ('gui/icon/*.png',       'gui/icon'),
                    ('gui/logs/*.log',       'gui/logs'),
                    ('/anaconda/lib/python2.7/site-packages/tvtk', 'tvtk'),
    		         ('/anaconda/lib/python2.7/site-packages/mayavi', 'mayavi'),
		         ('/anaconda/lib/python2.7/site-packages/pyface', 'pyface')],
             hiddenimports=['scipy.linalg.cython_blas',
			    'scipy.linalg.cython_lapack',
 			    'traitsui.qt4',
			    'pyface.ui.qt4.init',
			    'pyface.ui.qt4.action.menu_manager',
			    'pyface.ui.qt4.action.menu_bar_manager',
			    'tvtk.pyface.ui.qt4.init'],
             hookspath=None,
             runtime_hooks=['rthook_pyqt4.py'],
             excludes=None,
             win_no_prefer_redirects=None,
             win_private_assemblies=None,
             cipher=block_cipher)

pyz = PYZ(a.pure, a.zipped_data,
             cipher=block_cipher)
exe = EXE(pyz,
          a.scripts,
          exclude_binaries=True,
          name='jemris',
          debug=True,
          strip=None,
          upx=True,
          console=True)
coll = COLLECT(exe,
               a.binaries,
               a.zipfiles,
               a.datas,
               strip=None,
               upx=True,
               name='jemris')

