/****************************************************************************
** Meta object code from reading C++ file 'PlotWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "PlotWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PlotWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CPlotWidget_t {
    QByteArrayData data[22];
    char stringdata0[260];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CPlotWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CPlotWidget_t qt_meta_stringdata_CPlotWidget = {
    {
QT_MOC_LITERAL(0, 0, 11), // "CPlotWidget"
QT_MOC_LITERAL(1, 12, 14), // "doneZoomToRect"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 17), // "doneSelectingRect"
QT_MOC_LITERAL(4, 46, 2), // "rt"
QT_MOC_LITERAL(5, 49, 14), // "regionSelected"
QT_MOC_LITERAL(6, 64, 12), // "pointClicked"
QT_MOC_LITERAL(7, 77, 1), // "p"
QT_MOC_LITERAL(8, 79, 6), // "bshift"
QT_MOC_LITERAL(9, 86, 13), // "pointSelected"
QT_MOC_LITERAL(10, 100, 1), // "n"
QT_MOC_LITERAL(11, 102, 12), // "pointDragged"
QT_MOC_LITERAL(12, 115, 13), // "draggingStart"
QT_MOC_LITERAL(13, 129, 11), // "draggingEnd"
QT_MOC_LITERAL(14, 141, 22), // "backgroundImageChanged"
QT_MOC_LITERAL(15, 164, 13), // "OnZoomToWidth"
QT_MOC_LITERAL(16, 178, 14), // "OnZoomToHeight"
QT_MOC_LITERAL(17, 193, 11), // "OnZoomToFit"
QT_MOC_LITERAL(18, 205, 11), // "OnShowProps"
QT_MOC_LITERAL(19, 217, 17), // "OnCopyToClipboard"
QT_MOC_LITERAL(20, 235, 9), // "OnBGImage"
QT_MOC_LITERAL(21, 245, 14) // "OnClearBGImage"

    },
    "CPlotWidget\0doneZoomToRect\0\0"
    "doneSelectingRect\0rt\0regionSelected\0"
    "pointClicked\0p\0bshift\0pointSelected\0"
    "n\0pointDragged\0draggingStart\0draggingEnd\0"
    "backgroundImageChanged\0OnZoomToWidth\0"
    "OnZoomToHeight\0OnZoomToFit\0OnShowProps\0"
    "OnCopyToClipboard\0OnBGImage\0OnClearBGImage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CPlotWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       9,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   94,    2, 0x06 /* Public */,
       3,    1,   95,    2, 0x06 /* Public */,
       5,    1,   98,    2, 0x06 /* Public */,
       6,    2,  101,    2, 0x06 /* Public */,
       9,    1,  106,    2, 0x06 /* Public */,
      11,    1,  109,    2, 0x06 /* Public */,
      12,    1,  112,    2, 0x06 /* Public */,
      13,    1,  115,    2, 0x06 /* Public */,
      14,    0,  118,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      15,    0,  119,    2, 0x0a /* Public */,
      16,    0,  120,    2, 0x0a /* Public */,
      17,    0,  121,    2, 0x0a /* Public */,
      18,    0,  122,    2, 0x0a /* Public */,
      19,    0,  123,    2, 0x0a /* Public */,
      20,    0,  124,    2, 0x0a /* Public */,
      21,    0,  125,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QRect,    4,
    QMetaType::Void, QMetaType::QRect,    4,
    QMetaType::Void, QMetaType::QPointF, QMetaType::Bool,    7,    8,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void, QMetaType::QPoint,    7,
    QMetaType::Void, QMetaType::QPoint,    7,
    QMetaType::Void, QMetaType::QPoint,    7,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CPlotWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CPlotWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->doneZoomToRect(); break;
        case 1: _t->doneSelectingRect((*reinterpret_cast< QRect(*)>(_a[1]))); break;
        case 2: _t->regionSelected((*reinterpret_cast< QRect(*)>(_a[1]))); break;
        case 3: _t->pointClicked((*reinterpret_cast< QPointF(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 4: _t->pointSelected((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->pointDragged((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 6: _t->draggingStart((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 7: _t->draggingEnd((*reinterpret_cast< QPoint(*)>(_a[1]))); break;
        case 8: _t->backgroundImageChanged(); break;
        case 9: _t->OnZoomToWidth(); break;
        case 10: _t->OnZoomToHeight(); break;
        case 11: _t->OnZoomToFit(); break;
        case 12: _t->OnShowProps(); break;
        case 13: _t->OnCopyToClipboard(); break;
        case 14: _t->OnBGImage(); break;
        case 15: _t->OnClearBGImage(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CPlotWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CPlotWidget::doneZoomToRect)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CPlotWidget::*)(QRect );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CPlotWidget::doneSelectingRect)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (CPlotWidget::*)(QRect );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CPlotWidget::regionSelected)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (CPlotWidget::*)(QPointF , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CPlotWidget::pointClicked)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (CPlotWidget::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CPlotWidget::pointSelected)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (CPlotWidget::*)(QPoint );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CPlotWidget::pointDragged)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (CPlotWidget::*)(QPoint );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CPlotWidget::draggingStart)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (CPlotWidget::*)(QPoint );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CPlotWidget::draggingEnd)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (CPlotWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CPlotWidget::backgroundImageChanged)) {
                *result = 8;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CPlotWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CPlotWidget.data,
    qt_meta_data_CPlotWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CPlotWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CPlotWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CPlotWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int CPlotWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void CPlotWidget::doneZoomToRect()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void CPlotWidget::doneSelectingRect(QRect _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CPlotWidget::regionSelected(QRect _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void CPlotWidget::pointClicked(QPointF _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void CPlotWidget::pointSelected(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void CPlotWidget::pointDragged(QPoint _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void CPlotWidget::draggingStart(QPoint _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void CPlotWidget::draggingEnd(QPoint _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void CPlotWidget::backgroundImageChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}
struct qt_meta_stringdata_CDlgPlotWidgetProps_t {
    QByteArrayData data[1];
    char stringdata0[20];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CDlgPlotWidgetProps_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CDlgPlotWidgetProps_t qt_meta_stringdata_CDlgPlotWidgetProps = {
    {
QT_MOC_LITERAL(0, 0, 19) // "CDlgPlotWidgetProps"

    },
    "CDlgPlotWidgetProps"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CDlgPlotWidgetProps[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void CDlgPlotWidgetProps::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject CDlgPlotWidgetProps::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_CDlgPlotWidgetProps.data,
    qt_meta_data_CDlgPlotWidgetProps,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CDlgPlotWidgetProps::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CDlgPlotWidgetProps::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CDlgPlotWidgetProps.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int CDlgPlotWidgetProps::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
