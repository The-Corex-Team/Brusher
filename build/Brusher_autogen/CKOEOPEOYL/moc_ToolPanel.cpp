/****************************************************************************
** Meta object code from reading C++ file 'ToolPanel.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.2.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/ui/panels/ToolPanel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ToolPanel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.2.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ToolPanel_t {
    const uint offsetsAndSize[16];
    char stringdata0[117];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(offsetof(qt_meta_stringdata_ToolPanel_t, stringdata0) + ofs), len 
static const qt_meta_stringdata_ToolPanel_t qt_meta_stringdata_ToolPanel = {
    {
QT_MOC_LITERAL(0, 9), // "ToolPanel"
QT_MOC_LITERAL(10, 15), // "penToolSelected"
QT_MOC_LITERAL(26, 0), // ""
QT_MOC_LITERAL(27, 18), // "eraserToolSelected"
QT_MOC_LITERAL(46, 16), // "fillToolSelected"
QT_MOC_LITERAL(63, 22), // "eyedropperToolSelected"
QT_MOC_LITERAL(86, 16), // "lineToolSelected"
QT_MOC_LITERAL(103, 13) // "onToolClicked"

    },
    "ToolPanel\0penToolSelected\0\0"
    "eraserToolSelected\0fillToolSelected\0"
    "eyedropperToolSelected\0lineToolSelected\0"
    "onToolClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ToolPanel[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   50,    2, 0x06,    1 /* Public */,
       3,    0,   51,    2, 0x06,    2 /* Public */,
       4,    0,   52,    2, 0x06,    3 /* Public */,
       5,    0,   53,    2, 0x06,    4 /* Public */,
       6,    0,   54,    2, 0x06,    5 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       7,    0,   55,    2, 0x08,    6 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void ToolPanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ToolPanel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->penToolSelected(); break;
        case 1: _t->eraserToolSelected(); break;
        case 2: _t->fillToolSelected(); break;
        case 3: _t->eyedropperToolSelected(); break;
        case 4: _t->lineToolSelected(); break;
        case 5: _t->onToolClicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ToolPanel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ToolPanel::penToolSelected)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ToolPanel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ToolPanel::eraserToolSelected)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (ToolPanel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ToolPanel::fillToolSelected)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (ToolPanel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ToolPanel::eyedropperToolSelected)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (ToolPanel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ToolPanel::lineToolSelected)) {
                *result = 4;
                return;
            }
        }
    }
    (void)_a;
}

const QMetaObject ToolPanel::staticMetaObject = { {
    QMetaObject::SuperData::link<QDockWidget::staticMetaObject>(),
    qt_meta_stringdata_ToolPanel.offsetsAndSize,
    qt_meta_data_ToolPanel,
    qt_static_metacall,
    nullptr,
qt_incomplete_metaTypeArray<qt_meta_stringdata_ToolPanel_t
, QtPrivate::TypeAndForceComplete<ToolPanel, std::true_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>, QtPrivate::TypeAndForceComplete<void, std::false_type>
, QtPrivate::TypeAndForceComplete<void, std::false_type>


>,
    nullptr
} };


const QMetaObject *ToolPanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ToolPanel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ToolPanel.stringdata0))
        return static_cast<void*>(this);
    return QDockWidget::qt_metacast(_clname);
}

int ToolPanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void ToolPanel::penToolSelected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ToolPanel::eraserToolSelected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ToolPanel::fillToolSelected()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void ToolPanel::eyedropperToolSelected()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void ToolPanel::lineToolSelected()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
