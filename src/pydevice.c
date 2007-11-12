/*
 * pydevice.c
 *
 * Copyright (C) 2007  Red Hat, Inc.
 * All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions of
 * the GNU General Public License v.2, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY expressed or implied, including the implied warranties of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.  You should have received a copy of the
 * GNU General Public License along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.  Any Red Hat trademarks that are incorporated in the
 * source code or documentation are not subject to the GNU General Public
 * License and may only be used or replicated with the express permission of
 * Red Hat, Inc.
 *
 * Red Hat Author(s): David Cantrell <dcantrell@redhat.com>
 *                    Chris Lumens <clumens@redhat.com>
 */

#include <Python.h>

#include "convert.h"
#include "pyconstraint.h"
#include "pydevice.h"

/* _ped.CHSGeometry functions */
void _ped_CHSGeometry_dealloc(_ped_CHSGeometry *self) {
    PyObject_Del(self);
}

PyObject *_ped_CHSGeometry_new(PyTypeObject *type, PyObject *args,
                               PyObject *kwds) {
    _ped_CHSGeometry *self;

    self = PyObject_New(_ped_CHSGeometry, &_ped_CHSGeometry_Type_obj);
    return (PyObject *) self;
}

int _ped_CHSGeometry_init(_ped_CHSGeometry *self, PyObject *args,
                          PyObject *kwds) {
    /* XXX - should handle keywords (cylinders=, heads=, sectors=) */
    return 0;
}

PyObject *_ped_CHSGeometry_get(_ped_CHSGeometry *self, char *member) {
    if (member == NULL) {
        return NULL;
    }

    if (!strcmp(member, "cylinders")) {
        return Py_BuildValue("i", self->cylinders);
    } else if (!strcmp(member, "heads")) {
        return Py_BuildValue("i", self->heads);
    } else if (!strcmp(member, "sectors")) {
        return Py_BuildValue("i", self->sectors);
    } else {
        return NULL;
    }
}

/* _ped.Device functions */
void _ped_Device_dealloc(_ped_Device *self) {
    PyObject_Del(self);
}

PyObject *_ped_Device_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    _ped_Device *self;

    self = PyObject_New(_ped_Device, &_ped_Device_Type_obj);
    return (PyObject *) self;
}

int _ped_Device_init(_ped_Device *self, PyObject *args, PyObject *kwds) {
    /* XXX */
    return 0;
}

PyObject *_ped_Device_get(_ped_Device *self, char *member) {
    if (member == NULL) {
        return NULL;
    }

    if (!strcmp(member, "model")) {
        return Py_BuildValue("s", self->model);
    } else if (!strcmp(member, "path")) {
        return Py_BuildValue("s", self->path);
    } else if (!strcmp(member, "type")) {
        return PyLong_FromLongLong(self->type);
    } else if (!strcmp(member, "sector_size")) {
        return PyLong_FromLongLong(self->sector_size);
    } else if (!strcmp(member, "phys_sector_size")) {
        return PyLong_FromLongLong(self->phys_sector_size);
    } else if (!strcmp(member, "length")) {
        return PyLong_FromLongLong(self->length);
    } else if (!strcmp(member, "open_count")) {
        return Py_BuildValue("i", self->open_count);
    } else if (!strcmp(member, "read_only")) {
        return Py_BuildValue("i", self->read_only);
    } else if (!strcmp(member, "external_mode")) {
        return Py_BuildValue("i", self->external_mode);
    } else if (!strcmp(member, "dirty")) {
        return Py_BuildValue("i", self->dirty);
    } else if (!strcmp(member, "boot_dirty")) {
        return Py_BuildValue("i", self->boot_dirty);
    } else if (!strcmp(member, "host")) {
        return Py_BuildValue("h", self->host);
    } else if (!strcmp(member, "did")) {
        return Py_BuildValue("h", self->did);
    } else {
        return NULL;
    }
}

/* 1:1 function mappings for device.h in libparted */
PyObject *py_ped_device_probe_all(PyObject *s, PyObject *args)  {
    char *path;

    if (!PyArg_ParseTuple(args, "s", &path)) {
        return NULL;
    }

    ped_device_probe_all();

    free(path);

    Py_INCREF(Py_None);
    return Py_None;
}

PyObject *py_ped_device_free_all(PyObject *s, PyObject *args) {
    if (!PyArg_ParseTuple(args, "")) {
        return NULL;
    }

    ped_device_free_all();

    Py_INCREF(Py_None);
    return Py_None;
}

PyObject *py_ped_device_get(PyObject *s, PyObject *args) {
    PedDevice *device;
    _ped_Device *ret;
    char *path;

    if (!PyArg_ParseTuple(args, "s", &path)) {
        return NULL;
    }

    device = ped_device_get(path);
    if (device) {
        ret = PedDevice2_ped_Device(device);
    }

    free(path);
    ped_device_destroy(device);

    return (PyObject *) ret;
}

PyObject *py_ped_device_get_next(PyObject *s, PyObject *args) {
    PyObject *in_device;
    PedDevice *out_device, *device;
    _ped_Device *ret;

    if (!PyArg_ParseTuple(args, "|O!", &_ped_Device_Type_obj, &in_device)) {
        return NULL;
    }

    if (!in_device)
       out_device = NULL;
    else
       out_device = _ped_Device2PedDevice(in_device);

    device = ped_device_get_next(out_device);
    if (device) {
        ret = PedDevice2_ped_Device(device);
    }

    ped_device_destroy(out_device);
    ped_device_destroy(device);

    return (PyObject *) ret;
}

PyObject *py_ped_device_is_busy(PyObject *s, PyObject *args) {
    int ret = -1;
    PyObject *in_device;
    PedDevice *out_device;

    if (!PyArg_ParseTuple(args, "O!", &_ped_Device_Type_obj, &in_device)) {
        return NULL;
    }

    out_device = _ped_Device2PedDevice(in_device);
    if (out_device == NULL) {
        return NULL;
    }

    ret = ped_device_is_busy(out_device);
    ped_device_destroy(out_device);
    return PyBool_FromLong(ret);
}

PyObject *py_ped_device_open(PyObject *s, PyObject *args) {
    int ret = -1;
    PyObject *in_device;
    PedDevice *out_device;

    if (!PyArg_ParseTuple(args, "O!", &_ped_Device_Type_obj, &in_device)) {
        return NULL;
    }

    out_device = _ped_Device2PedDevice(in_device);
    if (out_device == NULL) {
        return NULL;
    }

    ret = ped_device_open(out_device);
    ped_device_destroy(out_device);
    return PyBool_FromLong(ret);
}

PyObject *py_ped_device_close(PyObject *s, PyObject *args) {
    int ret = -1;
    PyObject *in_device;
    PedDevice *out_device;

    if (!PyArg_ParseTuple(args, "O!", &_ped_Device_Type_obj, &in_device)) {
        return NULL;
    }

    out_device = _ped_Device2PedDevice(in_device);
    if (out_device == NULL) {
        return NULL;
    }

    ret = ped_device_close(out_device);
    ped_device_destroy(out_device);
    return PyBool_FromLong(ret);
}

PyObject *py_ped_device_destroy(PyObject *s, PyObject *args) {
    PyObject *in_device;
    PedDevice *out_device;

    if (!PyArg_ParseTuple(args, "O!", &_ped_Device_Type_obj, &in_device)) {
        return NULL;
    }

    out_device = _ped_Device2PedDevice(in_device);
    if (out_device == NULL) {
        return NULL;
    }

    ped_device_destroy(out_device);

    Py_INCREF(Py_None);
    return Py_None;
}

PyObject *py_ped_device_cache_remove(PyObject *s, PyObject *args) {
    PyObject *in_device;
    PedDevice *out_device;

    if (!PyArg_ParseTuple(args, "O!", &_ped_Device_Type_obj, &in_device)) {
        return NULL;
    }

    out_device = _ped_Device2PedDevice(in_device);
    if (out_device == NULL) {
        return NULL;
    }

    ped_device_cache_remove(out_device);

    ped_device_destroy(out_device);

    Py_INCREF(Py_None);
    return Py_None;
}

PyObject *py_ped_device_begin_external_access(PyObject *s, PyObject *args) {
    int ret = -1;
    PyObject *in_device;
    PedDevice *out_device;

    if (!PyArg_ParseTuple(args, "O!", &_ped_Device_Type_obj, &in_device)) {
        return NULL;
    }

    out_device = _ped_Device2PedDevice(in_device);
    if (out_device == NULL) {
        return NULL;
    }

    ret = ped_device_begin_external_access(out_device);

    ped_device_destroy(out_device);

    return PyBool_FromLong(ret);
}

PyObject *py_ped_device_end_external_access(PyObject *s, PyObject *args) {
    int ret = -1;
    PyObject *in_device;
    PedDevice *out_device;

    if (!PyArg_ParseTuple(args, "O!", &_ped_Device_Type_obj, &in_device)) {
        return NULL;
    }

    out_device = _ped_Device2PedDevice(in_device);
    if (out_device == NULL) {
        return NULL;
    }

    ret = ped_device_end_external_access(out_device);

    ped_device_destroy(out_device);

    return PyBool_FromLong(ret);
}

PyObject *py_ped_device_read(PyObject *s, PyObject *args) {
    PyObject *in_dev, *in_buf;
    PedSector start, count, ret;
    PedDevice *out_dev;
    void *out_buf;

    if (!PyArg_ParseTuple(args, "O!Oll", &_ped_Device_Type_obj, &in_dev,
                          &in_buf, &start, &count)) {
        return NULL;
    }

    out_dev = _ped_Device2PedDevice(in_dev);
    if (out_dev == NULL) {
        return NULL;
    }

    out_buf = PyCObject_AsVoidPtr(in_buf);
    if (out_buf == NULL) {
        return NULL;
    }

    ret = ped_device_read(out_dev, out_buf, start, count);
    ped_device_destroy(out_dev);

    return PyLong_FromLongLong(ret);
}

PyObject *py_ped_device_write(PyObject *s, PyObject *args) {
    PyObject *in_dev, *in_buf;
    PedSector start, count, ret;
    PedDevice *out_dev;
    void *out_buf;

    if (!PyArg_ParseTuple(args, "O!Oll", &_ped_Device_Type_obj, &in_dev,
                          &in_buf, &start, &count)) {
        return NULL;
    }

    out_dev = _ped_Device2PedDevice(in_dev);
    if (out_dev == NULL) {
        return NULL;
    }

    out_buf = PyCObject_AsVoidPtr(in_buf);
    if (out_buf == NULL) {
        return NULL;
    }

    ret = ped_device_write(out_dev, out_buf, start, count);
    ped_device_destroy(out_dev);

    return PyLong_FromLongLong(ret);
}

PyObject *py_ped_device_sync(PyObject *s, PyObject *args) {
    int ret = -1;
    PyObject *in_device;
    PedDevice *out_device;

    if (!PyArg_ParseTuple(args, "O!", &_ped_Device_Type_obj, &in_device)) {
        return NULL;
    }

    out_device = _ped_Device2PedDevice(in_device);
    if (out_device == NULL) {
        return NULL;
    }

    ret = ped_device_sync(out_device);

    ped_device_destroy(out_device);

    return PyBool_FromLong(ret);
}

PyObject *py_ped_device_sync_fast(PyObject *s, PyObject *args) {
    int ret = -1;
    PyObject *in_device;
    PedDevice *out_device;

    if (!PyArg_ParseTuple(args, "O!", &_ped_Device_Type_obj, &in_device)) {
        return NULL;
    }

    out_device = _ped_Device2PedDevice(in_device);
    if (out_device == NULL) {
        return NULL;
    }

    ret = ped_device_sync_fast(out_device);

    ped_device_destroy(out_device);

    return PyBool_FromLong(ret);
}

PyObject *py_ped_device_check(PyObject *s, PyObject *args) {
    PyObject *in_dev, *in_buf;
    PedSector start, count, ret;
    PedDevice *out_dev;
    void *out_buf;

    if (!PyArg_ParseTuple(args, "O!Oll", &_ped_Device_Type_obj, &in_dev,
                          &in_buf, &start, &count)) {
        return NULL;
    }

    out_dev = _ped_Device2PedDevice(in_dev);
    if (out_dev == NULL) {
        return NULL;
    }

    out_buf = PyCObject_AsVoidPtr(in_buf);
    if (out_buf == NULL) {
        return NULL;
    }

    ret = ped_device_check(out_dev, out_buf, start, count);
    ped_device_destroy(out_dev);

    return PyLong_FromLongLong(ret);
}

PyObject *py_ped_device_get_constraint(PyObject *s, PyObject *args) {
    PyObject *in_device;
    PedDevice *out_device;
    PedConstraint *constraint;
    _ped_Constraint *ret;

    if (!PyArg_ParseTuple(args, "O!", &_ped_Device_Type_obj, &in_device)) {
        return NULL;
    }

    out_device = _ped_Device2PedDevice(in_device);
    if (out_device == NULL) {
        return NULL;
    }

    constraint = ped_device_get_constraint(out_device);
    if (constraint) {
        ret = PedConstraint2_ped_Constraint(constraint);
    }

    ped_device_destroy(out_device);
    ped_constraint_destroy(constraint);

    return (PyObject *) ret;
}
