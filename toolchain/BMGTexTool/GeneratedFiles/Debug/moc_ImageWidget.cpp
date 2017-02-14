/****************************************************************************
** Meta object code from reading C++ file 'ImageWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../ImageWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ImageWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ImageWidget_t {
    QByteArrayData data[26];
    char stringdata0[257];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ImageWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ImageWidget_t qt_meta_stringdata_ImageWidget = {
    {
QT_MOC_LITERAL(0, 0, 11), // "ImageWidget"
QT_MOC_LITERAL(1, 12, 16), // "imageAreaChanged"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 10), // "ImageArea*"
QT_MOC_LITERAL(4, 41, 5), // "image"
QT_MOC_LITERAL(5, 47, 10), // "imageAdded"
QT_MOC_LITERAL(6, 58, 13), // "imageSelected"
QT_MOC_LITERAL(7, 72, 12), // "imageDeleted"
QT_MOC_LITERAL(8, 85, 12), // "dataModified"
QT_MOC_LITERAL(9, 98, 1), // "m"
QT_MOC_LITERAL(10, 100, 17), // "updateWindowTitle"
QT_MOC_LITERAL(11, 118, 16), // "adjustWindowSize"
QT_MOC_LITERAL(12, 135, 13), // "setClientSize"
QT_MOC_LITERAL(13, 149, 12), // "getImageRect"
QT_MOC_LITERAL(14, 162, 4), // "int&"
QT_MOC_LITERAL(15, 167, 5), // "width"
QT_MOC_LITERAL(16, 173, 6), // "height"
QT_MOC_LITERAL(17, 180, 12), // "drawToDevice"
QT_MOC_LITERAL(18, 193, 13), // "QPaintDevice*"
QT_MOC_LITERAL(19, 207, 6), // "device"
QT_MOC_LITERAL(20, 214, 6), // "select"
QT_MOC_LITERAL(21, 221, 13), // "combinToImage"
QT_MOC_LITERAL(22, 235, 7), // "QImage*"
QT_MOC_LITERAL(23, 243, 9), // "pickImage"
QT_MOC_LITERAL(24, 253, 1), // "x"
QT_MOC_LITERAL(25, 255, 1) // "y"

    },
    "ImageWidget\0imageAreaChanged\0\0ImageArea*\0"
    "image\0imageAdded\0imageSelected\0"
    "imageDeleted\0dataModified\0m\0"
    "updateWindowTitle\0adjustWindowSize\0"
    "setClientSize\0getImageRect\0int&\0width\0"
    "height\0drawToDevice\0QPaintDevice*\0"
    "device\0select\0combinToImage\0QImage*\0"
    "pickImage\0x\0y"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ImageWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   79,    2, 0x06 /* Public */,
       5,    1,   82,    2, 0x06 /* Public */,
       6,    1,   85,    2, 0x06 /* Public */,
       7,    1,   88,    2, 0x06 /* Public */,
       8,    1,   91,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    0,   94,    2, 0x08 /* Private */,
      11,    0,   95,    2, 0x08 /* Private */,
      12,    0,   96,    2, 0x08 /* Private */,
      13,    2,   97,    2, 0x08 /* Private */,
      17,    2,  102,    2, 0x08 /* Private */,
      17,    1,  107,    2, 0x28 /* Private | MethodCloned */,
      21,    0,  110,    2, 0x08 /* Private */,
      23,    2,  111,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::Bool,    9,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 14, 0x80000000 | 14,   15,   16,
    QMetaType::Void, 0x80000000 | 18, QMetaType::Bool,   19,   20,
    QMetaType::Void, 0x80000000 | 18,   19,
    0x80000000 | 22,
    0x80000000 | 3, QMetaType::Int, QMetaType::Int,   24,   25,

       0        // eod
};

void ImageWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ImageWidget *_t = static_cast<ImageWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->imageAreaChanged((*reinterpret_cast< ImageArea*(*)>(_a[1]))); break;
        case 1: _t->imageAdded((*reinterpret_cast< ImageArea*(*)>(_a[1]))); break;
        case 2: _t->imageSelected((*reinterpret_cast< ImageArea*(*)>(_a[1]))); break;
        case 3: _t->imageDeleted((*reinterpret_cast< ImageArea*(*)>(_a[1]))); break;
        case 4: _t->dataModified((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->updateWindowTitle(); break;
        case 6: _t->adjustWindowSize(); break;
        case 7: _t->setClientSize(); break;
        case 8: _t->getImageRect((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 9: _t->drawToDevice((*reinterpret_cast< QPaintDevice*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 10: _t->drawToDevice((*reinterpret_cast< QPaintDevice*(*)>(_a[1]))); break;
        case 11: { QImage* _r = _t->combinToImage();
            if (_a[0]) *reinterpret_cast< QImage**>(_a[0]) = _r; }  break;
        case 12: { ImageArea* _r = _t->pickImage((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< ImageArea**>(_a[0]) = _r; }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ImageWidget::*_t)(ImageArea * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ImageWidget::imageAreaChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (ImageWidget::*_t)(ImageArea * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ImageWidget::imageAdded)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (ImageWidget::*_t)(ImageArea * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ImageWidget::imageSelected)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (ImageWidget::*_t)(ImageArea * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ImageWidget::imageDeleted)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (ImageWidget::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ImageWidget::dataModified)) {
                *result = 4;
                return;
            }
        }
    }
}

const QMetaObject ImageWidget::staticMetaObject = {
    { &QScrollArea::staticMetaObject, qt_meta_stringdata_ImageWidget.data,
      qt_meta_data_ImageWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ImageWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ImageWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ImageWidget.stringdata0))
        return static_cast<void*>(const_cast< ImageWidget*>(this));
    if (!strcmp(_clname, "ImageOp"))
        return static_cast< ImageOp*>(const_cast< ImageWidget*>(this));
    return QScrollArea::qt_metacast(_clname);
}

int ImageWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QScrollArea::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void ImageWidget::imageAreaChanged(ImageArea * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ImageWidget::imageAdded(ImageArea * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ImageWidget::imageSelected(ImageArea * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ImageWidget::imageDeleted(ImageArea * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ImageWidget::dataModified(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
