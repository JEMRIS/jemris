#"""
#This program is free software; you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation; either version 2 of the License, or
#(at your option) any later version.
#
#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License
#along with this program; if not, write to the Free Software
#Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#"""
#
#from pyface.qt import QtGui
#
#from traits.api import HasTraits
#from traits.api import Instance
#from traits.api import Array
#from traits.api import on_trait_change
#
#from traitsui.api import View
#from traitsui.api import Item
#from traitsui.api import HGroup
#from traitsui.api import Group
#
#from tvtk.api import tvtk
#from tvtk.pyface.scene import Scene
#from mayavi import mlab
#from mayavi.core.api import PipelineBase
#from mayavi.core.api import Source
#from mayavi.core.ui.api import SceneEditor
##from mayavi.core.ui.api import MayaviScene
#from mayavi.core.ui.api import MlabSceneModel
#
#import sip
#
## The object implementing the dialog
#class VolumeSlicer(HasTraits):
#    # The data to plot
#    data = Array()
#    label = Array()
#
#    # The 4 views displayed
#    scene3d = Instance(MlabSceneModel, ())
#    scene_x = Instance(MlabSceneModel, ())
#    scene_y = Instance(MlabSceneModel, ())
#    scene_z = Instance(MlabSceneModel, ())
#
#    # The data source
#    data_src3d = Instance(Source)
#    label_src3d = Instance(Source)
#
#    # The image plane widgets of the 3D scene
#    ipw_3d_x = Instance(PipelineBase)
#    ipw_3d_y = Instance(PipelineBase)
#    ipw_3d_z = Instance(PipelineBase)
#
#    _axis_names = dict(x=0, y=1, z=2)
#
#    # -------------------------------------------------------------------------
#    def __init__(self, **traits):
#        super(VolumeSlicer, self).__init__(**traits)
#        # Force the creation of the image_plane_widgets:
#        self.ipw_3d_x
#        self.ipw_3d_y
#        self.ipw_3d_z
#
#    # -------------------------------------------------------------------------
#    # Default values
#    # -------------------------------------------------------------------------
#    def _data_src3d_default(self):
#
#        return mlab.pipeline.scalar_field(self.current_data(),
#                                          figure=self.scene3d.mayavi_scene)
#
#    def _label_src3d_default(self):
#        return mlab.pipeline.scalar_field(self.label,
#                                          figure=self.scene3d.mayavi_scene)
#
#    def make_ipw_3d(self, axis_name):
#        " Planes through 3d volume "
#        ipw = mlab.pipeline.image_plane_widget(
#                                    self.data_src3d,
#                                    figure=self.scene3d.mayavi_scene,
#                                    plane_orientation='%s_axes' % axis_name,
#                                    transparent=True)  # mzimmermann
#        return ipw
#
#    def _ipw_3d_x_default(self):
#        return self.make_ipw_3d('x')
#
#    def _ipw_3d_y_default(self):
#        return self.make_ipw_3d('y')
#
#    def _ipw_3d_z_default(self):
#        return self.make_ipw_3d('z')
#
#    ###########################################################################
#
#    def update_label(self, label):
#
#        self.label_src3d.mlab_source.set(scalars=label)
#
#    def update_data(self, data):
#
#        self.data = data
#        self.flush_data()
#
#    def current_data(self):
#
#        if len(self.data.shape) == 3:
#            scalars = self.data
#        else:
#            scalars = self.data[..., self.index]
#        return scalars
#
#    def flush_data(self):
#
#        self.data_src3d.mlab_source.set(scalars=self.current_data())
#        self.flush_module_manager()
#
#    def update_index(self, index):
#
#        self.index = index
#        self.flush_data()
#
#    def update_module_manager(self, lut_mode, data_range):
#
#        self.lut_mode = lut_mode        # Color Map
#        self.data_range = data_range
#        self.flush_module_manager()
#
#    def flush_module_manager(self):
#
#        for axis in ['x', 'y', 'z']:
#            ipw = getattr(self, 'ipw_%s' % axis)
#            ipw.module_manager.scalar_lut_manager.set(
#                                                lut_mode=self.lut_mode,
#                                                data_range=self.data_range)
#            ipw.module_manager.source.update()
#
#        # TODO: Not clear why this is necessary (actually any update does)
#        self.data_src3d.mlab_source.update()
#
#    ###########################################################################
#
#    # -------------------------------------------------------------------------
#    # Scene activation callbacks
#    # -------------------------------------------------------------------------
#    @on_trait_change('scene3d.activated')
#    def display_scene3d(self):
#
#        " Contour Surface "
#        outline = mlab.pipeline.outline(self.label_src3d,
#                                        figure=self.scene3d.mayavi_scene)
#        mlab.pipeline.contour_surface(outline, contours=2, transparent=True,
#                                      color=(0.5, 0.5, 0.5))
#
#        self.scene3d.mlab.view(40, 50)
#
#        # Interaction properties can only be changed after the scene
#        # has been created, and thus the interactor exists
#        for ipw in (self.ipw_3d_x, self.ipw_3d_y, self.ipw_3d_z):
#            # Turn the interaction off
#            ipw.ipw.interaction = 0
#        self.scene3d.scene.background = (0, 0, 0)
#        # Keep the view always pointing up
#        self.scene3d.scene.interactor.interactor_style = tvtk.InteractorStyleTerrain()
#
#    def make_side_view(self, axis_name):
#        scene = getattr(self, 'scene_%s' % axis_name)
#
#        # To avoid copying the data, we take a reference to the
#        # raw VTK dataset, and pass it on to mlab. Mlab will create
#        # a Mayavi source from the VTK without copying it.
#        # We have to specify the figure so that the data gets
#        # added on the figure we are interested in.
#        " Side Views "
#        outline = mlab.pipeline.outline(
#                            self.data_src3d.mlab_source.dataset,
#                            figure=scene.mayavi_scene)
#        ipw = mlab.pipeline.image_plane_widget(
#                            outline)
#
#        ipw.ipw.plane_orientation = '%s_axes' % axis_name
#
#        ipw.module_manager.scalar_lut_manager.set(lut_mode=self.lut_mode)
#        # Causes bad window warning
#        ipw.module_manager.scalar_lut_manager.show_scalar_bar = True
#        ipw.module_manager.scalar_lut_manager.use_default_range = False
#
#        setattr(self, 'ipw_%s' % axis_name, ipw)
#
#        # Synchronize positions between the corresponding image plane
#        # widgets on different views.
#        ipw.ipw.sync_trait('slice_position',
#                           getattr(self, 'ipw_3d_%s' % axis_name).ipw)
#
#        # Make left-clicking create a crosshair
#        ipw.ipw.left_button_action = 0
#
#        # Add a callback on the image plane widget interaction to
#        # move the others
#        def move_view(obj, evt):
#            position = obj.GetCurrentCursorPosition()
#            for other_axis, axis_number in self._axis_names.iteritems():
#                if other_axis == axis_name:
#                    continue
#                ipw3d = getattr(self, 'ipw_3d_%s' % other_axis)
#                ipw3d.ipw.slice_position = position[axis_number]
#
#        ipw.ipw.add_observer('InteractionEvent', move_view)
#        ipw.ipw.add_observer('StartInteractionEvent', move_view)
#
#        # Center the image plane widget
#        ipw.ipw.slice_position = 0.5*self.data.shape[
#                    self._axis_names[axis_name]]
#
#        # Position the view for the scene
#        views = dict(x=( 0, 90),
#                     y=(90, 90),
#                     z=( 0,  0),
#                     )
#        scene.mlab.view(*views[axis_name])
#        # 2D interaction: only pan and zoom
#        # TODO: no interactor style
#        scene.scene.interactor.interactor_style = tvtk.InteractorStyleImage()
#        scene.scene.background = (0, 0, 0)
#
#        scene.mlab.title(self.scene_titles[['x', 'y', 'z'].index(axis_name)])
#
#    @on_trait_change('scene_x.activated')
#    def display_scene_x(self):
#        return self.make_side_view('x')
#
#    @on_trait_change('scene_y.activated')
#    def display_scene_y(self):
#        return self.make_side_view('y')
#
#    @on_trait_change('scene_z.activated')
#    def display_scene_z(self):
#        return self.make_side_view('z')
#
#    # -------------------------------------------------------------------------
#    # The layout of the dialog created
#    # -------------------------------------------------------------------------
#    view = View(Group(
#                  HGroup(
#                       Item('scene_x', editor=SceneEditor(scene_class=Scene)),
#                       Item('scene_y', editor=SceneEditor(scene_class=Scene)),
#                       show_labels=False,
#                  ),
#                  HGroup(
#                       Item('scene_z', editor=SceneEditor(scene_class=Scene)),
#                       Item('scene3d', editor=SceneEditor(scene_class=Scene)),
#                       show_labels=False,
#                  ),
#                ),
#                resizable=True,
#                )
#
#
#class QVolumeSlicer(QtGui.QWidget):
#
#    def __init__(self):
#
#        QtGui.QWidget.__init__(self)
#        self.visualization = None
#        self.shape = None
#
#    def init(self, label, data, cmap):
#
#        scene_titles = ['Coronal Plane', 'Sagittal Plane', 'Transversal Plane']
#
#        self.visualization = VolumeSlicer(data=data,
#                                          label=label,
#                                          lut_mode=cmap,
#                                          index=0,
#                                          data_range=[data.min(), data.max()],
#                                          scene_titles=scene_titles)
#
#        self.ui = self.visualization.edit_traits(parent=self,
#                                                 kind='subpanel').control
#
#        layout = QtGui.QGridLayout(margin=0)
#        layout.addWidget(self.ui)
#        self.setLayout(layout)
#
#    def update(self, label, data, cmap):
#
#        label = (label > 0).astype(int)
#
#        if self.visualization is None:
#
#            self.init(label, data, cmap)
#
#        elif not (self.visualization.label.shape == label.shape):
#
#            # TODO: Make more clean solution to
#            # change size of data than reinit.
#            self.init(label, data, cmap)
#
#        else:
#
#            self.visualization.update_label(label)
#            self.update_data(data, cmap)
#
#    def update_data(self, data, cmap):
#
#        data_range = [data.min(), data.max()]
#
#        self.visualization.update_data(data)
#        self.visualization.update_module_manager(cmap, data_range)
#
#    def update_index(self, index):
#
#        self.visualization.update_index(index)
#
#    def setLayout(self, layout):
#
#        # Clear layout
#        if self.layout() is not None:
#            old_layout = self.layout()
#            for i in reversed(range(old_layout.count())):
#                old_layout.itemAt(i).widget().setParent(None)
#            sip.delete(old_layout)
#
#        # Set layout
#        super(QVolumeSlicer, self).setLayout(layout)


from nibabel.viewers import OrthoSlicer3D
import matplotlib.pyplot as plt
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg
from PyQt5 import QtGui, QtWidgets


class QVolumeSlicer(QtWidgets.QWidget):

    def __init__(self):

        QtWidgets.QWidget.__init__(self)
#        self.visualization = None
#        self.shape = None

        self.fig = plt.figure()
        self.canvas = FigureCanvasQTAgg(self.fig)
        
        layout = QtWidgets.QGridLayout(margin=0)
        layout.addWidget(self.canvas)
        self.setLayout(layout)
        
        self.ax = [self.fig.add_subplot(2, 2, i+1) for i in range(4)]

    def init(self, label, data, cmap):
        
        self.orthoslicer = OrthoSlicer3D(data, axes=self.ax)
        
        pass

#        scene_titles = ['Coronal Plane', 'Sagittal Plane', 'Transversal Plane']
#
#        self.visualization = VolumeSlicer(data=data,
#                                          label=label,
#                                          lut_mode=cmap,
#                                          index=0,
#                                          data_range=[data.min(), data.max()],
#                                          scene_titles=scene_titles)
#
#        self.ui = self.visualization.edit_traits(parent=self,
#                                                 kind='subpanel').control
#
#        layout = QtGui.QGridLayout(margin=0)
#        layout.addWidget(self.ui)
#        self.setLayout(layout)

    def update(self, label, data, cmap):
        
        self.orthoslicer = OrthoSlicer3D(data, axes=self.ax)
        
        pass

#        label = (label > 0).astype(int)
#
#        if self.visualization is None:
#
#            self.init(label, data, cmap)
#
#        elif not (self.visualization.label.shape == label.shape):
#
#            # TODO: Make more clean solution to
#            # change size of data than reinit.
#            self.init(label, data, cmap)
#
#        else:
#
#            self.visualization.update_label(label)
#            self.update_data(data, cmap)

    def update_data(self, data, cmap):

        self.orthoslicer = OrthoSlicer3D(data, axes=self.ax)
        
        pass

#        data_range = [data.min(), data.max()]
#
#        self.visualization.update_data(data)
#        self.visualization.update_module_manager(cmap, data_range)

    def update_index(self, index):
        
        pass

#        self.visualization.update_index(index)

    def setLayout(self, layout):

        pass
        
#        # Clear layout
#        if self.layout() is not None:
#            old_layout = self.layout()
#            for i in reversed(range(old_layout.count())):
#                old_layout.itemAt(i).widget().setParent(None)
#            sip.delete(old_layout)
#
#        # Set layout
#        super(QVolumeSlicer, self).setLayout(layout)

