"""
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
"""

import math
import time
import copy

from PyQt5 import QtGui, QtCore, QtWidgets
from PyQt5.QtCore import QTimeLine


class CoreQTreeWidget(QtWidgets.QTreeWidget):

    itemMoved = QtCore.pyqtSignal(list, list)
    itemDeleted = QtCore.pyqtSignal()
    itemCopy = QtCore.pyqtSignal()

    def __init__(self, tableWidget, icons, showHidden):

        super(CoreQTreeWidget, self).__init__()
        self.setDragDropMode(self.InternalMove)

        self.tableWidget = tableWidget
        self.icons = icons
        self.showHidden = showHidden        # CheckBox

        self.itemClicked.connect(self.treeItemClicked)
        self.itemMoved.connect(self.treeItemMove)
        self.itemDeleted.connect(self.treeItemDelete)
        self.itemCopy.connect(self.treeItemCopy)

        self.showHidden.stateChanged.connect(self.treeItemClicked)

        self.tableWidget.setItemDelegate(ViewDelegate(self))

        self.header().hide()

    def update(self, seq):

        self.xml = seq
        self.fill_widget()

    def fill_item(self, Qroot, Xroot):

        Qroot.setExpanded(True)

        Qchild = QTreeWidgetItem()
        if 'Name' in Xroot.attrib.keys():
            Qchild.setText(0, '{0} [{1}]'.format(Xroot.attrib['Name'],
                                                 Xroot.tag))
            Qchild.setName(Xroot.attrib['Name'])
            Qchild.setType(Xroot.get('type'))
        else:
            Qchild.setText(0, '[{0}]'.format(Xroot.tag))
            Qchild.setName(None)
            Qchild.setType(Xroot.get('type'))

        font = QtGui.QFont()
        font.setBold(True)
        Qchild.setFont(0, font)
        if Xroot.get('type') in ['PULSE', 'COIL']:
            Qchild.setIcon(0, QtGui.QIcon(self.icons[Xroot.tag.upper()]))
            Qchild.setForeground(0, QtGui.QColor('black'))
        else:
            Qchild.setForeground(0, QtGui.QColor('white'))
        bgc = QtGui.QColor(Xroot.get('color'))  # background color
        bgc.setAlpha(127)           # transparency
        Qchild.setBackground(0, bgc)
        Qroot.addChild(Qchild)

        for Xchild in Xroot:
            self.fill_item(Qchild, Xchild)

    def fill_widget(self):

        self.clear()
        self.tableWidget.fullClear()
        self.fill_item(self.invisibleRootItem(), self.xml.get())

    def treeItemDelete(self):

        if self.currentItem() is None:
            return

        listIdx = self.currentItem().idx()
        self.xml.delete(listIdx)
        self.fill_widget()

    def treeItemCopy(self):

        if self.currentItem() is None:
            return

        listIdx = self.currentItem().idx()
        self.xml.copy(listIdx)
        self.fill_widget()

    def treeItemInsert(self):

        if self.currentItem() is None or self.sender() is None:
            return

        tag = str(self.sender().text())
        listIdx = self.currentItem().idx()
        msg = self.xml._insert(listIdx, tag)
        if msg is not None:
            msgBox = QtGui.QMessageBox()
            msgBox.setText(msg)
            msgBox.exec_()

        self.fill_widget()

    def treeItemMove(self, dragIndex, dropIndex):

        msg = self.xml.move(dragIndex, dropIndex)
        if msg is not None:
            msgBox = QtGui.QMessageBox()
            msgBox.setText(msg)
            msgBox.exec_()
        self.fill_widget()

    def treeItemClicked(self):

        if self.currentItem() is None:
            return

        listIdx = self.currentItem().idx()
        showHidden = self.showHidden.isChecked()

        keys, values, tag = self.xml.returnAttr(listIdx, showHidden)

        self.tableWidget.clear()
        self.tableWidget.setRowCount(len(keys))
        self.tableWidget.setColumnCount(1)
        self.tableWidget.setHorizontalHeaderLabels([tag])
        self.tableWidget.horizontalHeader().show()
        self.tableWidget.horizontalHeader().setSectionResizeMode(
                QtWidgets.QHeaderView.Stretch)
        self.tableWidget.setVerticalHeaderLabels(keys)

        for idx, value in enumerate(values):
            if value is not None:
                Qitem = QtWidgets.QTableWidgetItem(value)
                self.tableWidget.setItem(idx, 0, Qitem)

    def tableChanged(self):

        currentTableItem = self.tableWidget.currentItem()

        # Note: editing finished is emitted twice. The second time,
        # qtablewidget looses focus (currentItem = None) and an error occurs.
        if currentTableItem is None:
            return

        row = currentTableItem.row()
        key = str(self.tableWidget.verticalHeaderItem(row).text())
        val = str(self.tableWidget.item(row, 0).text())

        currentTreeItem = self.currentItem()
        idx = currentTreeItem.idx()

        name, tag = self.xml.writeAttr(idx, key, val)

        # Update Name in QTreeWidget
        currentTreeItem.setText(0, '{0} [{1}]'.format(name, tag))
        currentTreeItem.setName(name)

    def dropEvent(self, event):

        if event.source() == self:

            item = self.currentItem()
            dragIndex = item.idx()
            # default implementation
            QtWidgets.QTreeWidget.dropEvent(self, event)
            dropIndex = item.idx()

            if dropIndex is None:
                return

            self.emit(QtCore.SIGNAL('itemMoved'), dragIndex, dropIndex)

    def keyPressEvent(self, event):

        if event.key() == QtCore.Qt.Key_Delete:
            self.itemDeleted.emit()


class QTreeWidget(CoreQTreeWidget):

    def __init__(self, tableWidget, icons, showHidden):

        super(QTreeWidget, self).__init__(tableWidget, icons, showHidden)
        self.setHorizontalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)
        self.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)

        self.setAcceptDrops(True)
        self.dragModSoon = False  # dragMod just starts after 0.1 sec
        self.started = 0  # to take the time of dragModSoon

        self.dragSource = None
        self.dragMod = False
        self.copyDrag = False
        self.lastp = QtCore.QPoint(0, 0)  # Last position of the dragged item

        # USED FOR PREVIEW #

        # idx of a temporary moved/inserted item (not Voiditem)
        self.callback = None
        # Used for the drag and drop preview
        self.stillInItem = None
        self.voididxlist = []

        # SIZES(some handeld automatically) #

        self.circleSize = 1.5/2.  # Scaling factor for the nodes
        self.rw = 0
        self.rh = 0
        self.minr = 0

        # ANIMATION HELP #

        self.lineList_old = []
        self.lineList_new = []

        self.nodeCopy = []  # dictionarys filled with information

        self.old_opacity = 1
        self.timeline = None
        self.animationDuration = 200
        self.needUpdate = False

    def getItemList(self):
        """Returns a list of the items (Depth-first search [preordering])"""
        itemlist = []
        self.rekursivHelp(self.topLevelItem(0), itemlist)
        return itemlist

    def rekursivHelp(self, item, list):
        """Recursive help for the getItemList method"""
        list.append(item)
        for i in range(item.childCount()):
            self.rekursivHelp(item.child(i), list)

    def tableChanged(self):
        super(QTreeWidget, self).tableChanged()
        self.needUpdate = True

    def paintEvent(self, event):
        """Painting the view, if there is a root item"""

        if self.itemAt(0, 0) is None:
            return

        painter = QPainter(self.viewport())

        self.makeImage(painter)

        # drawing the dragged item
        if (self.dragMod and self.dragSource == self.source and
                self.currentItem() is not None):
            pos = self.lastp - QtCore.QPoint(self.minr/4, self.minr/4)
            self.paintItem(self.currentItem(), pos,
                           painter, self.xml)
            if self.timeline is None:
                # paintItem saves the drawn node. But the dragged Node is not
                # fixed thats why I remove it again.
                del self.nodeCopy[-1]

        painter.end()

    def makeImage(self, painter):
        """
        This method draws the image to a given painter.
        The algorithm works like this:

        - Find out how the items have to be placed, relativly
        - Take the relativ height and width of the tree
        - Now, set the position with absolute values
        - Save the lines
        - Check if some transitions have to be made:
            - if there is no transition required at the moment
            - and the lines changed or needUpdate flag is set

        For the tranistion take a look at the methods
        'animate' and 'animationEnd'.
        """

        painter.setRenderHint(QPainter.TextAntialiasing, True)
        painter.setRenderHint(QPainter.Antialiasing, True)

        itemList = self.getItemList()

        wDSF = 1  # width Dependend Scaling Factor
        hDSF = 1  # height Dependend Scaling Factor

        width = self.width()
        height = self.height()

        # The depth of the Tree
        maxlevel = 4
        # Max width in a Tree level
        maxlevelwidth = 1

        # Setting up maxlevel & item.level values
        for item in itemList:
            item.pos = QtCore.QPoint(0, 0)
            if item.parent() is not None:
                if item.getType() not in ['PULSE']:
                    item.level = item.parent().level+1
                    if item.level > maxlevel:
                        maxlevel = item.level

        # List of the width in a tree level
        levelwidth = [0] * len(itemList)

        for item in itemList:
            if item.parent() is not None:
                # This way, we dont write a node over the child of another node
                for j in range(item.level, len(levelwidth)):
                    if levelwidth[item.level] < levelwidth[j]:
                        levelwidth[item.level] = levelwidth[j]
                # Setting up the Item position
                item.pos = QtCore.QPoint(levelwidth[item.level], item.level)

                # A child cant be on the left side of a parent node
                if item.pos.x() < item.parent().pos.x():
                    item.pos = QtCore.QPoint(item.parent().pos.x(),
                                             item.pos.y())
                    levelwidth[item.level] = item.pos.x()

                # PULSE nodes don't grow to the side
                if item.getType() not in ['PULSE']:
                    levelwidth[item.level] += 1

                # To handle multiply PULSE nodes, we take their parent node
                if item.getType() in ['ATOMICSEQUENCE']:
                    if item.childCount() != 0:
                        # Multiple PULSE nodes takes 1 width in a tree level
                        levelwidth[item.level+1] += 1
                    # Multiple PULES nodes increasing the tree depth
                    if (item.level + item.childCount()) > maxlevel:
                        maxlevel = (item.level + item.childCount())

        # Getting the maxlevelwidth
        maxlevelwidth = max(levelwidth)

        if maxlevelwidth == 0:
            maxlevelwidth = 1

        self.rw = width/(maxlevelwidth) * wDSF
        self.rh = height/(maxlevel+1) * hDSF  # +1 is needed, to make it fit

        self.minr = self.rw if self.rw < self.rh else self.rh

        # ------------------------------------------------------------------- #
        # Positioning #
        # ------------------------------------------------------------------- #

        for item in itemList:
            if item.getType() not in ['PULSE']:
                qpoint = QtCore.QPoint(self.minr/6 + 3, 3)
                item.pos = QtCore.QPoint(item.pos.x()*self.rw,
                                         item.pos.y()*self.rh) + qpoint
                if item.getType() in ['ATOMICSEQUENCE']:
                    for i in range(item.childCount()):
                        child = item.child(i)
                        child.pos = QtCore.QPoint(
                                item.pos.x(), item.pos.y() +
                                (i+1)*self.minr*self.circleSize)

        # ------------------------------------------------------------------- #
        # Save Lines #
        # ------------------------------------------------------------------- #

        painter.linePainter(item)

        self.lineList_new = [[]]
        for item in itemList:
            # No lines between ATOMIC... and PULSE
            if item.getType() not in ['ATOMICSEQUENCE', 'COILARRAY']:
                for i in range(item.childCount()):
                    if not item.child(i).visible:
                        continue
                    self.lineList_new += [[
                            item.pos +
                            QtCore.QPoint(self.minr*self.circleSize/2,
                                          self.minr*self.circleSize/2),
                            item.child(i).pos +
                            QtCore.QPoint(self.minr*self.circleSize/2,
                                          self.minr*self.circleSize/2)]]

        # ------------------------------------------------------------------- #
        # possible Animation start #
        # ------------------------------------------------------------------- #

        if (self.timeline is None and
                (self.lineList_new[:] != self.lineList_old[:] or
                 self.needUpdate)):
            self.needUpdate = False
            self.timeline = QTimeLine()
            self.timeline.valueChanged.connect(self.animate)
            self.timeline.finished.connect(self.animationEnd)
            self.timeline.setDuration(self.animationDuration)
            self.timeline.start()

        # ------------------------------------------------------------------- #
        # Drawing Lines #
        # ------------------------------------------------------------------- #

        painter.setOpacity(self.old_opacity)
        for line in self.lineList_old:
            if line != []:
                painter.drawLine(line[0], line[1])

        if self.old_opacity != 1:       # with Animation
            painter.setOpacity(1-self.old_opacity)
            for line in self.lineList_new:
                if line != []:
                    painter.drawLine(line[0], line[1])

        # ------------------------------------------------------------------- #
        # Drawing Nodes #
        # ------------------------------------------------------------------- #

        painter.setOpacity(1)
        if self.timeline is None:       # without Animation
            if len(self.nodeCopy) == 0:
                tmp = copy.deepcopy(self.xml)
                for item in itemList:     # drawing and saving new nodes
                    if item is not None and item.visible:
                        self.paintItem(item, item.pos, painter, tmp)
            else:
                for cp in self.nodeCopy:  # using old nodes for drawing
                    self.paintItem2(cp, painter)

        else:                           # with Animation
            painter.setOpacity(self.old_opacity)
            for cp in self.nodeCopy:
                self.paintItem2(cp, painter)
            painter.setOpacity(1-self.old_opacity)
            for item in itemList:
                if item is not None and item.visible:
                    self.paintItem(item, item.pos, painter, self.xml)

    def animationEnd(self):
        """
        AnimationEnd cleans up the animation. Called by QTimeLine.
        """
        self.lineList_old = self.lineList_new
        self.lineList_new = None
        self.old_opacity = 1.0
        self.nodeCopy = []      # removing old nodes
        self.timeline = None
        self.updateImage()

    def animate(self, value):
        """
        Animate makes a little change for the tranistion. Called by QTimeLine.
        """
        self.old_opacity = 1.0 - value
        self.updateImage()

    def calcVoidItems(self):
        """
        This method creates the Voiditems for the preview.
        """
        if not self.dragMod:
            return

        # ------------------------------------------------------------------- #
        # Calculating VoidItems indexes #
        # ------------------------------------------------------------------- #

        self.voididxlist = []
        for item in self.getItemList():
            if item.getType() not in ['PULSE']:
                idx = item.idx() + [item.childCount()]
                if self.checkVoidItem(idx):
                    self.voididxlist += [idx]

        dragIndex = None
        if self.currentItem() is not None:  # saving the current item idx
            dragIndex = self.currentItem().idx()

        # ------------------------------------------------------------------- #
        # Creation of Voiditems #
        # ------------------------------------------------------------------- #

        for voididx in self.voididxlist:
            if self.dragSource == self.source:  # not using button drag
                if self.callback is not None:       # item not moved/copyd
                    self.setCurrentItem(self.getItem(self.callback))
#
                    self.itemCopy.emit()
                    self.itemMoved.emit(self.callback, voididx)
                else:                               # item moved/copyd
                    if voididx is self.voididxlist[0]:
                        dragIndex[-1] += 1
                        self.itemCopy.emit()
                        self.itemMoved.emit(dragIndex, voididx)
                        dragIndex[-1] -= 1
                        self.setCurrentItem(self.getItem(dragIndex))
                    else:
                        self.setCurrentItem(self.getItem(self.voididxlist[0]))
                        self.itemCopy.emit()
                        self.itemMoved.emit(self.voididxlist[0], voididx)
            else:                                # using button drag
                if self.callback is None:       # item not inserted
                    if voididx is self.voididxlist[0]:
                        self.insertItem(voididx)
                    else:
                        self.setCurrentItem(self.getItem(self.voididxlist[0]))
                        self.itemCopy.emit()
                        self.itemMoved.emit(self.voididxlist[0], voididx)
                else:                           # item already inserted
                    self.setCurrentItem(self.getItem(self.callback))
                    self.itemCopy.emit()
                    self.itemMoved.emit(self.callback, voididx)

        if dragIndex is not None:       # setting the current item back
            self.setCurrentItem(self.getItem(dragIndex))

        self.markVoidItems()    # finishing void items

    def checkVoidItem(self, idx):
        """
        Checks, if an idx can be taken for a new VoidItem. First some special
        cases are caught and then the standard checkDrop function is used.
        """

        if self.callback is not None:
            if len(idx) == len(self.callback):
                numb = len(self.callback)-1
                if (idx[0:numb] == self.callback[0:numb] and
                        idx[numb] >= self.callback[numb]):
                    return False
            else:
                if len(idx) >= len(self.callback):
                    numb = len(self.callback)
                    if idx[0:numb] == self.callback[0:numb]:
                        return False
        return self.checkDrop(idx)

    def setBranchVoid(self, root, isVoid):
        """
        This Method sets the isVoid flag of a hole branch to the given
        boolean value in 'isVoid'.
        """

        change = [root]
        for node in change:
            node.isVoid = isVoid
            for i in range(node.childCount()):
                change += [node.child(i)]

    def checkDrop(self, dropidx):
        """
        This Methods checks, if a (copy)move/insertion of an item is
        legal.
        """

        dropIndex = dropidx[:]
        tmp = copy.deepcopy(self.xml)
        if self.dragSource == self.source:
            dragIndex = self.currentItem().idx()
            if self.copyDrag:
                tmp.copy(dragIndex)
                dragIndex[-1] += 1
            else:
                if len(dropIndex) >= len(dragIndex):
                    numb = len(dragIndex)
                    if dragIndex[0:numb] <= dropIndex[0:numb]:
                        if dropIndex[0:numb] == dragIndex[0:numb]:
                            return False
                        dropIndex[numb-1] -= 1
            try:
                tmp._move(dragIndex, dropIndex)
                status, msg = tmp.checkCommand(dropIndex)
                if msg is not None:
                    return False
            # exception caused, when tmp.checkCommand wont return 2 values
            except:
                return False
        else:
            tag = self.dragSource.text()
            idx = dropIndex[0:-1]

            if len(idx) == 0:
                return False

            tmp = copy.deepcopy(self.xml)
            msg = tmp._insert(idx, tag)
            if msg is not None:
                return False
        return True

    def deleteVoidItems(self):
        """
        Removs all Items, which are VoidItems.
        """

        savedidx = None
        if self.currentItem() is not None:
            savedidx = self.currentItem().idx()
        for idx in self.voididxlist:
            self.setCurrentItem(self.getItem(idx))
            self.treeItemDelete()
        if savedidx is not None:
            self.setCurrentItem(self.getItem(savedidx))
        self.voididxlist = []

    def paintItem(self, item, pos, painter, xml):
        """
        This method is a preparation for the item painting in 'paintItem2'.
        The infomation, which is required to draw an item, is saved if
        needed (for the animation).
        """

        informlist = {
                "pos": pos, "text": item.getName(),
                "backgroundColor": item.background(0).color(),
                "isVoid": item.isVoid,
                "type": item.getType(),
                "iscurrent": item is self.currentItem(),
                "idx": item.idx(),
                "xml": xml,
                "textcolor": item.foreground(0).color(),
                "font": item.font(0),
                "minr": self.minr}

        if self.timeline is None:
            self.nodeCopy.append(informlist)

        self.paintItem2(informlist, painter)

    def paintItem2(self, itemInfos, painter):
        """
        In this method, the items are painted. The attribute 'iteminfos'
        should contain a dictionary with the needed informtion.
        """

        minSizeForPulsPic = 25

        sIC = QtGui.QColor(0, 150, 200)  # selected item color

        sICBW = 2  # selected item color border width

        minr = itemInfos["minr"]

        # ------------------------------------------------------------------- #
        # Draw Form #
        # ------------------------------------------------------------------- #

        # drawshape = needed shape
        # rect = Postion + size of the shape

        if itemInfos["type"] in ['PULSE', 'COIL']:
            rect = QtCore.QRect(itemInfos["pos"].x()-minr/6,
                                itemInfos["pos"].y(),
                                minr*self.circleSize + self.minr/3,
                                minr*self.circleSize)
            drawshape = painter.drawRect
        else:
            rect = QtCore.QRect(itemInfos["pos"].x(), itemInfos["pos"].y(),
                                minr*self.circleSize,
                                minr*self.circleSize)
            drawshape = painter.drawEllipse

        # shape color
        colorB = itemInfos["backgroundColor"]
        if self.callback is not None:
            if ((self.dragSource == self.source and
                 itemInfos["idx"] == self.callback and
                 not itemInfos["iscurrent"]) or
                (self.dragSource != self.source and
                 itemInfos["iscurrent"])):
                colorB = QtGui.QColor(220, 220, 220)

        # alphaf(1) to overdraw the lines, which connects the nodes
        # TODO:
        colorB.setAlphaF(1)
        if itemInfos["iscurrent"]:  # draw a blue border for the currentItem
            painter.setBrush(sIC)
            drawshape(QtCore.QRect(rect.x() - sICBW,
                                   rect.y() - sICBW,
                                   rect.width() + 2*sICBW,
                                   rect.height() + 2*sICBW))

        if itemInfos["isVoid"]:     # VoidItems are drawn with orange
            colorB = QtGui.QColor('orange')
            painter.formPainter(rect, None, colorB, itemtype=itemInfos["type"])
        else:
            painter.formPainter(rect, None, colorB, itemtype=itemInfos["type"])

        drawshape(rect)  # Drawing the shape

        # ------------------------------------------------------------------- #
        # Draw Icon #
        # ------------------------------------------------------------------- #

        painter.iconPainter()
        icon = None

        if minr >= minSizeForPulsPic:
            xitem = itemInfos["xml"].read(itemInfos["idx"])
            if xitem.get('type') in ['PULSE', 'COIL']:
                icon = self.icons[xitem.tag]

                painter.drawPixmap(
                        itemInfos["pos"] + QtCore.QPoint(0, minr * 1/7),
                        icon, QtCore.QRect(0, 0, icon.width(), icon.height()))

        # ------------------------------------------------------------------- #
        # Draw Text #
        # ------------------------------------------------------------------- #

        painter.textPainter(textColor=itemInfos["textcolor"],
                            font=itemInfos["font"])

        if icon is not None:
            painter.drawText(itemInfos["pos"] + QtCore.QPoint(
                    icon.width(), icon.height() + minr*1/7), itemInfos["text"])
        else:
            painter.drawText(QtCore.QRectF(rect), QtCore.Qt.AlignCenter,
                             itemInfos["text"])

    def pointInItem(self, p, item):
        """
        Checks if a point is in the shape of an Item. This is used to
        detect, if a user clicks or hovers over an item (possible drop
        action).
        """

        m = self.minr
        if item.getType() in ['PULSE']:     # Checks if p is in a Rectangle
            return (p.x() > item.pos.x()-m/6 and
                    p.y() > item.pos.y() and
                    p.x() < item.pos.x()-m/6-2+m*self.circleSize+m/3 and
                    p.y() < item.pos.y()+m*self.circleSize)
        else:                               # Checks if p is in a Circle
            return (math.sqrt((p.x()-item.pos.x()-m*self.circleSize/2)**2 +
                              (p.y()-item.pos.y()-m*self.circleSize/2)**2) <
                    m*self.circleSize/2)

    def mousePressEvent(self, e):
        self.lastp = e.pos()
        for item in self.getItemList():
            if self.pointInItem(self.lastp, item):
                self.setCurrentItem(item)
                self.treeItemClicked()
                self.dragModSoon = True
                self.started = time.time()
                self.nodeCopy = []
                self.updateImage()
                break

    def mouseReleaseEvent(self, e):
        self.lastp = e.pos()

        if self.dragMod:
            dropevent = QtGui.QDropEvent(e.pos(), QtCore.Qt.DropAction(),
                                         QtCore.QMimeData(),
                                         QtCore.Qt.MouseButton(),
                                         QtCore.Qt.KeyboardModifiers(),
                                         QtCore.QEvent.Drop)
            dropevent.source = self.source
            if self.callback is not None and self.callback in self.voididxlist:
                self.voididxlist.remove(self.callback)
            self.deleteVoidItems()

            if self.callback is not None:
                if self.dragSource == self.source and not self.copyDrag:
                    self.treeItemDelete()
            self.callback = None
            self.dragMod = False
            self.needUpdate = True
            self.updateImage()
        self.dragModSoon = False

    def mouseMoveEvent(self, event):
        e = QtGui.QMouseEvent(event)
        self.lastp = e.pos()

        if self.dragModSoon and not self.dragMod:
            if time.time() - self.started > 0.1:
                self.dragMod = True
                self.dragSource = self.source
                self.calcVoidItems()

        if self.dragMod:
            self.previewManager(event)
            self.updateImage()

    def stillInItemFunc(self, p):
        """
        This Method detects, if the cursor is still hovering over the
        shape, which causes a change. By making a move/copy/insert preview,
        it is possible, that the new shapes don't matches the cursor
        position, so that a preview would be volatile without this method.
        """

        m = self.stillInItem[2]
        itempos = self.stillInItem[1]

        if self.stillInItem[0] in ['PULSE']:    # p in Rectangle
            return (p.x() > itempos.x()-m/6 and
                    p.y() > itempos.y() and
                    p.x() < itempos.x()-m/6-2+m*self.circleSize+m/3 and
                    p.y() < itempos.y()+m*self.circleSize)
        else:                                   # p in Circle
            return (math.sqrt((p.x()-itempos.x()-m*self.circleSize/2)**2 +
                              (p.y()-itempos.y()-m*self.circleSize/2)**2) <
                    m*self.circleSize/2)

    def setBranchVisible(self, item, visible):
        """
        Changes the visibility of a complete branch. The given item should be
        the root of the branch. This method is used in the move preview
        (without copy).
        """

        change = [item]
        for node in change:
            node.visible = visible
            for i in range(node.childCount()):
                change += [node.child(i)]

    def previewManager(self, event):
        """
        This method decides, if a (copy)move/insert preview has to be done.
        It can also delete a preview, if the dragged item is not over a shape
        anymore.
        """

        # Limiting the the calls of this method
        if time.time() - self.started > 0.1:
            self.started = time.time()

            dropIndex = self.calcDropIndex(event.pos())
            if dropIndex is not None:               # Cursor over shape
                if self.callback is None:           # There is no preview
                    self.makePreview(event, dropIndex)  # make move/copy/insert
            elif (self.callback is not None and
                  not self.stillInItemFunc(event.pos())):
                self.deletePreview()            # undo move/copy/insert
                self.stillInItem = None

    def deletePreview(self):
        """
        This method deletes a preview, that is caused by dragging an item over
        another item.
        """

        self.deleteVoidItems()
        if self.dragSource == self.source:      # not caused by a button drag
            item = self.getItem(self.callback)
            self.setCurrentItem(item)
            self.treeItemDelete()
            self.callback = None
            self.setCurrentItem(self.getItem(self.copyhelp))
            if not self.copyDrag:
                self.currentItem().visible = True
        else:                                   # caused by a button drag
            self.setCurrentItem(self.getItem(self.callback))
            self.treeItemDelete()
            self.callback = None
        self.calcVoidItems()
        self.needUpdate = True

    def makePreview(self, event, dropIndex):
        """
        Checks and creates a new (copy)move/insert preview.
        """

        if not self.checkDrop(dropIndex):   # We can't drop the Item here
            return

        if self.dragSource == self.source:  # not caused by a button
            self.makePreviewCopyDrag(event, dropIndex)  # (copy)move
        else:                               # caused by a button
            self.makePreviewInsert(event, dropIndex)    # insert
        self.needUpdate = True  # repainting the image of the widget

    def makePreviewCopyDrag(self, event, dropIndex):
        """
        This Method creates a preview of a (copy)move action.
        """

        self.deleteVoidItems()

        dragIndex = self.currentItem().idx()
        self.callback = dropIndex
        event.source = self.source
        self.copydropEvent(event, dropIndex)
        self.copyhelp = dragIndex[:]

        if len(dragIndex) >= len(dropIndex):
            numb = len(dropIndex)-1
            if (dropIndex[0:numb] == dragIndex[0:numb] and
                    dropIndex[numb] <= dragIndex[numb]):
                dragIndex[numb] += 1

        self.setCurrentItem(self.getItem(dragIndex))
        self.calcVoidItems()
        if not self.copyDrag:
            self.setBranchVisible(self.currentItem(), False)
        self.paintactivated = True

    def makePreviewInsert(self, event, dropIndex):
        """
        This method creates a preview of a button drag action.
        """

        if dropIndex == [0]:    # you can't drop anything on the root item
            return

        self.deleteVoidItems()

        self.insertItem(dropIndex)
        new = self.getItem(dropIndex)
        self.setCurrentItem(new)
        self.callback = dropIndex

        self.calcVoidItems()

    def insertItem(self, dropIndex):
        """
        Insert an item as a consequence of a button action.
        """

        idx = dropIndex[0:-1]
        prep = self.getItem(idx)
        self.setCurrentItem(prep)
        idx += [prep.childCount()]
        self.dragSource.sendInsertSignal()
        self.itemMoved.emit(idx, dropIndex)
#        self.emit(QtCore.SIGNAL('itemMoved'), idx, dropIndex)

    def markVoidItems(self, dropIndex=None):
        """
        This method marks all VoidItems, which are children of a VoidItem.
        """

        for voididx in self.voididxlist:
            if dropIndex is not None and len(voididx) >= len(dropIndex):
                numb = len(dropIndex)
                if (dropIndex[0:numb] <= voididx[0:numb]):
                    voididx[numb-1] += 1
                    self.setBranchVoid(self.getItem(voididx), True)
                    voididx[numb-1] -= 1
            else:
                self.setBranchVoid(self.getItem(voididx), True)

    def getItem(self, idx):
        """
        Returns the item at a given item index.
        """

        for item in self.getItemList():
            if item.idx() == idx:
                return item

    def source(self):
        # just for creating the dropevent
        return self

    def updateImage(self):
        # forcing this widget to create a paintEvent
        self.viewport().repaint()

    def calcDropIndex(self, pos):
        """
        This methods finds out, if a dragged item is hovering over another
        item.
        """

        idx = None
        for item in self.getItemList():
            if self.pointInItem(pos, item):
                searched = item
                idx = searched.idx()
                if searched.isVoid:
                    for voididx in self.voididxlist:
                        if len(idx) >= len(voididx):
                            numb = len(voididx)
                            if idx[0:numb] == voididx[0:numb]:
                                idx = voididx
                self.stillInItem = [searched.getType(),
                                    searched.pos, self.minr]
                break

        return idx

    def copydropEvent(self, event, dropIndex=None):
        """
        Make a copy of an Item and then move it to the given dropIndex.
        """

        if event.source() == self:

            dragIndex = self.currentItem().idx()

            if dropIndex is None:
                dropIndex = self.calcDropIndex(event.pos())

            if dropIndex is None:
                return

            tmp = copy.deepcopy(self.xml)
            tmp.copy(dragIndex)
            dragIndex[-1] += 1
            tmp._move(dragIndex, dropIndex)
            status, msg = tmp.checkCommand(dropIndex)

            if status:
                self.itemCopy.emit()
                self.itemMoved.emit(dragIndex, dropIndex)

    def dragEnterEvent(self, e):  # button drag is over the widget
        e.accept()
        self.dragSource = e.source()
        self.dragMod = True
        self.calcVoidItems()
        self.paintactivated = True
        self.updateImage()

    def dragLeaveEvent(self, e):  # button drag leaves the widget
        self.deleteVoidItems()
        self.dragMod = False
        self.dragSource = None
        self.paintactivated = True
        self.updateImage()

    def dragMoveEvent(self, e):
        """
        This method gets called, when the button drag function is used.
        """

        self.previewManager(e)

    def dropEvent(self, event):

        if event.source() == self:      # not a button

            item = self.currentItem()
            dragIndex = item.idx()
            dropIndex = self.calcDropIndex(event.pos())

            if dropIndex is None:
                return

            self.itemMoved.emit(dragIndex, dropIndex)

        else:                           # a button
            if self.callback is None:
                for item in self.getItemList():
                    if self.pointInItem(event.pos(), item):
                        self.setCurrentItem(item)
                        self.treeItemInsert()
                        break
            self.callback = None
            self.dragMod = False
            self.deleteVoidItems()
            self.paintactivated = True
        self.dragMod = False

    def keyPressEvent(self, event):
        """
        Detects if the CopyDrag gets enabled.
        """

        if not self.dragMod:
            super(QTreeWidget, self).keyPressEvent(event)

        if event.key() == QtCore.Qt.Key_Shift:
            if self.dragMod:
                if self.callback is not None:
                    dropIndex = self.callback[:]
                    self.deletePreview()
                    self.copyDrag = True
                    self.makePreview(event, dropIndex)
                else:
                    self.deleteVoidItems()
                    self.copyDrag = True
                    self.calcVoidItems()
                self.paintactivated = True
            else:
                self.nodeCopy = []
                self.copyDrag = True
        self.updateImage()

    def keyReleaseEvent(self, event):
        """
        Detects if the CopyDrag gets disabled.
        """

        if event.key() == QtCore.Qt.Key_Shift:
            if self.dragMod:
                if self.callback is not None:
                    dropIndex = self.callback[:]
                    self.deletePreview()
                    self.copyDrag = False
                    self.makePreview(event, dropIndex)
                else:
                    self.deleteVoidItems()
                    self.copyDrag = False
                    self.calcVoidItems()
                self.needUpdate = True
            else:
                self.copyDrag = False
                self.nodeCopy = []
        self.updateImage()


class QPainter(QtGui.QPainter):

    def formPainter(self, rect, item, color, itemtype=None):

        if itemtype is None:
            itemtype = item.getType()
        pen = QtGui.QPen()
        if itemtype in ['PULSE']:
            pen.setColor(QtGui.QColor('black'))
        else:
            pen.setColor(QtGui.QColor('gray'))

        if itemtype in ['PULSE']:
            dc = 25
        else:
            dc = 75

        colorAt0 = [c-dc if c-dc > 0 else 0 for c in color.getRgb()]
        colorAt1 = [c+dc if c+dc < 255 else 255 for c in color.getRgb()]

        gradient = QtGui.QLinearGradient(rect.x(),
                                         rect.y(),
                                         rect.x()+rect.width(),
                                         rect.y()+rect.height())
        gradient.setColorAt(0.0, QtGui.QColor(colorAt0[0],
                                              colorAt0[1],
                                              colorAt0[2]))
        gradient.setColorAt(1.0, QtGui.QColor(colorAt1[0],
                                              colorAt1[1],
                                              colorAt1[2]))

        brush = QtGui.QBrush(gradient)

        self.setPen(pen)
        self.setBrush(brush)

    def textPainter(self, item=None, textColor=None, font=None):

        pen = QtGui.QPen()
        pen.setColor(item.textColor(0) if textColor is None else textColor)

        self.setPen(pen)
        self.setFont(item.font(0) if font is None else font)

    def iconPainter(self):

        pen = QtGui.QPen()
        pen.setColor(QtGui.QColor('black'))         # Icons drawn in black

        self.setPen(pen)

    def linePainter(self, item=None):

        pen = QtGui.QPen()
        pen.setColor(QtGui.QColor('black'))
        pen.setWidth(2)
        self.setPen(pen)


class QTreeWidgetItem(QtWidgets.QTreeWidgetItem):
    """
    QTreeWidgetItem with additional functionalities. It can find out its
    relative (row) and absolute (idx) position within a QTreeWidget. It also
    stores the name and type of each item.
    """

    def __init__(self):

        super(QTreeWidgetItem, self).__init__()
        self.pos = QtCore.QPoint(0, 0)
        self.level = 0
        self.visible = True
        self.isVoid = False

    def row(self):
        """
        Get current row of item by testing for self reference.
        """
        if self.parent() is not None:
            for idx in range(self.parent().childCount()+1):
                if self is self.parent().child(idx):
                    return idx
        else:
            # No parent means root.
            return 0

    def idx(self, idx=list()):
        """
        Get absolute index of item.
        """
        idx = [self.row()] + idx

        if self.parent() is not None:
            idx = self.parent().idx(idx)

        return idx

    def setType(self, _type):

        self.type = _type

    def setName(self, name):

        self.name = name

    def getType(self):

        return self.type

    def getName(self):

        return self.name


class QTableWidget(QtWidgets.QTableWidget):

    def fullClear(self):
        self.clear()
        self.setRowCount(0)
        self.horizontalHeader().hide()


class ViewDelegate(QtWidgets.QItemDelegate):

    def __init__(self, parent=None):
        super(ViewDelegate, self).__init__(parent)
        self.parent = parent

    def createEditor(self, parent, option, index):
        editor = super(ViewDelegate, self).createEditor(parent, option, index)

        if isinstance(editor, QtWidgets.QLineEdit):

            editor.editingFinished.connect(self.parent.tableChanged)

        return editor


class QProcess(QtCore.QProcess):

    def __init__(self, parent=None):

        super(QProcess, self).__init__(parent)

        # Merge stdout and stderr into stdout (interleaved).
        self.ProcessChannelMode(QtCore.QProcess.MergedChannels)

        self.readyReadStandardOutput.connect(self.readStdOutput)

    def setUpdateFunction(self, updateFunction):

        self.updateFunction = updateFunction

    def setExitFunction(self, exitFunction):

        self.exitFunction = exitFunction
        self.finished.connect(self.exitFunction)

    @QtCore.pyqtSlot()
    def readStdOutput(self):

        self.updateFunction(str(self.readAllStandardOutput()))


class QToolBar(QtWidgets.QToolBar):

    def __init__(self, icons, order, slot, parent=None):

        super(QToolBar, self).__init__(parent)

        for key in order:
            if key == 'SEPARATOR':
                self.addSeparator()
            else:
                draggable = key not in ['OPEN', 'NEW', 'SAVE', 'SAVEAS']
                button = QButton(QtGui.QIcon(icons[key]), key, slot, draggable)
                self.addWidget(button)


class QButton(QtWidgets.QPushButton):

    itemInsert = QtCore.pyqtSignal()

    def __init__(self, icon, string, slot, draggable=True, parent=None):

        super(QButton, self).__init__(icon, string, parent)
        self.itemInsert.connect(slot)
#        self.connect(self, QtCore.SIGNAL('itemInsert'), slot)
        self.setFlat(True)
        self.setText("")
        self.textinfo = string
        self.setMaximumWidth(50)
        self.setToolTip('Add <b>%s</b>' % string)
        self.draggable = draggable

    def text(self):
        return self.textinfo

    def mouseMoveEvent(self, e):
        if not self.draggable or e.buttons() != QtCore.Qt.LeftButton:
            return

        super(QButton, self).mouseReleaseEvent(e)
        mimeData = QtCore.QMimeData()
        drag = QtGui.QDrag(self)
        drag.setMimeData(mimeData)
        drag.setHotSpot(e.pos() - self.rect().topLeft())
        icon = self.icon()
        drag.setPixmap(icon.pixmap(30, 30))
#        painter = QtGui.QPainter()
#        painter.begin(drag.pixmap())
#        QTreeWidget.paintItem(QTreeWidgetItem(None), QtCore.)
#        painter.end()

#        drag.start(QtCore.Qt.MoveAction)
        drag.exec_(QtCore.Qt.MoveAction)
        self.clearFocus()

    def mousePressEvent(self, e):
        super(QButton, self).mousePressEvent(e)

    def mouseReleaseEvent(self, e):
        super(QButton, self).mouseReleaseEvent(e)
        self.sendInsertSignal()
        self.clearFocus()

    def sendInsertSignal(self):

        self.itemInsert.emit()


class QProgressBar(QtWidgets.QProgressBar):

    def setText(self, text):
        self._text = text

    def text(self):
        return self._text
