/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * gmpy_mpfr.h                                                             *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Python interface to the GMP or MPIR, MPFR, and MPC multiple precision   *
 * libraries.                                                              *
 *                                                                         *
 * Copyright 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007,               *
 *           2008, 2009 Alex Martelli                                      *
 *                                                                         *
 * Copyright 2008, 2009, 2010, 2011, 2012, 2013, 2014 Case Van Horsen      *
 *                                                                         *
 * This file is part of GMPY2.                                             *
 *                                                                         *
 * GMPY2 is free software: you can redistribute it and/or modify it under  *
 * the terms of the GNU Lesser General Public License as published by the  *
 * Free Software Foundation, either version 3 of the License, or (at your  *
 * option) any later version.                                              *
 *                                                                         *
 * GMPY2 is distributed in the hope that it will be useful, but WITHOUT    *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or   *
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public    *
 * License for more details.                                               *
 *                                                                         *
 * You should have received a copy of the GNU Lesser General Public        *
 * License along with GMPY2; if not, see <http://www.gnu.org/licenses/>    *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


/* gmpy_mpfr C API extension header file.
 *
 * Provide interface to the MPFR (Multiple Precision Floating-point with
 * Rounding) library.
 *
 * Version 2.00, April 2011 (created) casevh
 */

#ifndef GMPY_MPFR_H
#define GMPY_MPFR_H

#ifdef __cplusplus
extern "C" {
#endif


#if !defined(FLT_RADIX) || (FLT_RADIX!=2)
#   error "FLT_RADIX undefined or != 2, GMPY2 is confused. :("
#endif

#if defined(MS_WIN32) && defined(_MSC_VER)
#  pragma comment(lib,"mpfr.lib")
#endif

typedef struct {
    PyObject_HEAD
    mpfr_t f;
    Py_hash_t hash_cache;
    int rc;
    int round_mode;
} MPFR_Object;

#define GMPY_DIVZERO(msg) PyErr_SetString(GMPyExc_DivZero, msg)
#define GMPY_INEXACT(msg) PyErr_SetString(GMPyExc_Inexact, msg)
#define GMPY_INVALID(msg) PyErr_SetString(GMPyExc_Invalid, msg)
#define GMPY_OVERFLOW(msg) PyErr_SetString(GMPyExc_Overflow, msg)
#define GMPY_UNDERFLOW(msg) PyErr_SetString(GMPyExc_Underflow, msg)
#define GMPY_ERANGE(msg) PyErr_SetString(GMPyExc_Erange, msg)
#define GMPY_EXPBOUND(msg) PyErr_SetString(GMPyExc_ExpBound, msg)

#define CHECK_UNDERFLOW(msg) \
    if (mpfr_underflow_p() && (context->ctx.traps & TRAP_UNDERFLOW)) { \
        GMPY_UNDERFLOW(msg); \
        goto done; \
    }

#define MPFR_CHECK_UNDERFLOW(mpfrt, msg) \
    if (mpfr_zero_p(mpfrt->f) && mpfrt->rc) { \
        context->ctx.underflow = 1; \
        if (context->ctx.traps * TRAP_UNDERFLOW) { \
            GMPY_UNDERFLOW(msg); \
            goto done; \
        } \
    }

#define CHECK_OVERFLOW(msg) \
    if (mpfr_overflow_p() && (context->ctx.traps & TRAP_OVERFLOW)) { \
        GMPY_OVERFLOW(msg); \
        goto done; \
    }

#define MPFR_CHECK_OVERFLOW(mpfrt, msg) \
    if (mpfr_inf_p(mpfrt->f)) { \
        context->ctx.overflow = 1; \
        if (context->ctx.traps & TRAP_OVERFLOW) { \
            GMPY_OVERFLOW(msg); \
            goto done; \
        } \
    }

#define CHECK_INEXACT(msg) \
    if (mpfr_inexflag_p() && (context->ctx.traps & TRAP_INEXACT)) { \
        GMPY_INEXACT(msg); \
        goto done; \
    }

#define MPFR_CHECK_INEXACT(mpfrt, msg) \
    if (mpfrt->rc)  { \
        context->ctx.inexact = 1; \
        if (context->ctx.traps & TRAP_INEXACT) { \
            GMPY_INEXACT(msg); \
            goto done; \
        } \
    }

#define CHECK_INVALID(msg) \
    if (mpfr_nanflag_p() && (context->ctx.traps & TRAP_INVALID)) { \
        GMPY_INVALID(msg); \
        goto done; \
    }

#define MPFR_CHECK_INVALID(mpfrt, msg) \
    if (mpfr_nan_p(mpfrt->f)) { \
        context->ctx.invalid = 1; \
        if (context->ctx.traps & TRAP_INVALID) { \
            GMPY_INVALID(msg); \
            goto done; \
        } \
    }

#define CHECK_ERANGE(msg) \
    if (mpfr_erangeflag_p() && (context->ctx.traps & TRAP_ERANGE)) { \
        GMPY_ERANGE(msg); \
        goto done; \
    }

#define CHECK_DIVBY0(msg) \
    if (mpfr_divby0_p() && (context->ctx.traps & TRAP_DIVZERO)) { \
        GMPY_DIVZERO(msg); \
        goto done; \
    }

#define MERGE_FLAGS \
    context->ctx.underflow |= mpfr_underflow_p(); \
    context->ctx.overflow |= mpfr_overflow_p(); \
    context->ctx.invalid |= mpfr_nanflag_p(); \
    context->ctx.inexact |= mpfr_inexflag_p(); \
    context->ctx.erange |= mpfr_erangeflag_p(); \
    context->ctx.divzero |= mpfr_divby0_p();

#define CHECK_FLAGS(NAME) \
    CHECK_DIVBY0("'mpfr' division by zero in "NAME); \
    CHECK_INVALID("'mpfr' invalid operation in "NAME); \
    CHECK_UNDERFLOW("'mpfr' underflow in "NAME); \
    CHECK_OVERFLOW("'mpfr' overflow in "NAME); \
    CHECK_INEXACT("'mpfr' inexact result in "NAME); \

#define GMPY_MPFR_CHECK_RANGE(V, CTX) \
    if (mpfr_regular_p(V->f) && \
        (!((V->f->_mpfr_exp >= CTX->ctx.emin) && \
           (V->f->_mpfr_exp <= CTX->ctx.emax)))) { \
        mpfr_exp_t _oldemin, _oldemax; \
        _oldemin = mpfr_get_emin(); \
        _oldemax = mpfr_get_emax(); \
        mpfr_set_emin(CTX->ctx.emin); \
        mpfr_set_emax(CTX->ctx.emax); \
        V->rc = mpfr_check_range(V->f, V->rc, GET_MPFR_ROUND(CTX)); \
        mpfr_set_emin(_oldemin); \
        mpfr_set_emax(_oldemax); \
    }

#define GMPY_MPFR_SUBNORMALIZE(V, CTX) \
    if (CTX->ctx.subnormalize && \
        V->f->_mpfr_exp >= CTX->ctx.emin && \
        V->f->_mpfr_exp <= CTX->ctx.emin + mpfr_get_prec(V->f) - 2) { \
        mpfr_exp_t _oldemin, _oldemax; \
        _oldemin = mpfr_get_emin(); \
        _oldemax = mpfr_get_emax(); \
        mpfr_set_emin(CTX->ctx.emin); \
        mpfr_set_emax(CTX->ctx.emax); \
        V->rc = mpfr_subnormalize(V->f, V->rc, GET_MPFR_ROUND(CTX)); \
        mpfr_set_emin(_oldemin); \
        mpfr_set_emax(_oldemax); \
    }

/* Exceptions should be checked in order of least important to most important.
 */

#define GMPY_MPFR_EXCEPTIONS(V, CTX, NAME) \
    CTX->ctx.underflow |= mpfr_underflow_p(); \
    CTX->ctx.overflow |= mpfr_overflow_p(); \
    CTX->ctx.invalid |= mpfr_nanflag_p(); \
    CTX->ctx.inexact |= mpfr_inexflag_p(); \
    CTX->ctx.divzero |= mpfr_divby0_p(); \
    if (CTX->ctx.traps) { \
        if ((CTX->ctx.traps & TRAP_UNDERFLOW) && mpfr_underflow_p()) { \
            GMPY_UNDERFLOW(NAME" underflow"); \
            Py_XDECREF((PyObject*)V); \
            V = NULL; \
        } \
        if ((CTX->ctx.traps & TRAP_OVERFLOW) && mpfr_overflow_p()) { \
            GMPY_OVERFLOW(NAME" overflow"); \
            Py_XDECREF((PyObject*)V); \
            V = NULL; \
        } \
        if ((CTX->ctx.traps & TRAP_INEXACT) && mpfr_inexflag_p()) { \
            GMPY_INEXACT(NAME" inexact result"); \
            Py_XDECREF((PyObject*)V); \
            V = NULL; \
        } \
        if ((CTX->ctx.traps & TRAP_INVALID) && mpfr_nanflag_p()) { \
            GMPY_INVALID(NAME" invalid operation"); \
            Py_XDECREF((PyObject*)V); \
            V = NULL; \
        } \
        if ((CTX->ctx.traps & TRAP_DIVZERO) && mpfr_divby0_p()) { \
            GMPY_DIVZERO(NAME" division by zero"); \
            Py_XDECREF((PyObject*)V); \
            V = NULL; \
        } \
    }

#define GMPY_MPFR_CLEANUP(V, CTX, NAME) \
    GMPY_MPFR_CHECK_RANGE(V, CTX); \
    GMPY_MPFR_SUBNORMALIZE(V, CTX); \
    GMPY_MPFR_EXCEPTIONS(V, CTX, NAME); \

#define GMPY_CHECK_ERANGE(V, CTX, MSG) \
    CTX->ctx.erange |= mpfr_erangeflag_p(); \
    if (CTX->ctx.traps) { \
        if ((CTX->ctx.traps & TRAP_ERANGE) && mpfr_erangeflag_p()) { \
            GMPY_ERANGE(MSG); \
            Py_XDECREF((PyObject*)V); \
            V = NULL; \
        } \
    } \

#define MPFR_CHECK_FLAGS(mpfrt, NAME) \
    CHECK_DIVBY0(mpfrt, "'mpfr' division by zero in "NAME); \
    CHECK_INVALID(mpfrt, "'mpfr' invalid operation in "NAME); \
    CHECK_INEXACT(mpfrt, "'mpfr' inexact result in "NAME); \
    CHECK_UNDERFLOW(mpfrt, "'mpfr' underflow in "NAME); \
    CHECK_OVERFLOW(mpfrt, "'mpfr' overflow in "NAME); \

#define SUBNORMALIZE(NAME) \
    if (context->ctx.subnormalize) \
        NAME->rc = mpfr_subnormalize(NAME->f, NAME->rc, context->ctx.mpfr_round);

#define MPFR_SUBNORMALIZE(mpfrt) \
    if (context->ctx.subnormalize) \
        mpfrt->rc = mpfr_subnormalize(mpfrt->f, mpfrt->rc, context->ctx.mpfr_round);

#define MPFR_CLEANUP_SELF(NAME) \
    SUBNORMALIZE(result); \
    MERGE_FLAGS; \
    CHECK_DIVBY0("'mpfr' division by zero in "NAME); \
    CHECK_INVALID("'mpfr' invalid operation in "NAME); \
    CHECK_UNDERFLOW("'mpfr' underflow in "NAME); \
    CHECK_OVERFLOW("'mpfr' overflow in "NAME); \
    CHECK_INEXACT("'mpfr' inexact result in "NAME); \
  done: \
    Py_DECREF(self); \
    if (PyErr_Occurred()) { \
        Py_XDECREF((PyObject*)result); \
        result = NULL; \
    } \
    return (PyObject*)result;

#define MPFR_CLEANUP_SELF_OTHER(NAME) \
    SUBNORMALIZE(result); \
    MERGE_FLAGS; \
    CHECK_DIVBY0("'mpfr' division by zero in "NAME); \
    CHECK_INVALID("'mpfr' invalid operation in "NAME); \
    CHECK_UNDERFLOW("'mpfr' underflow in "NAME); \
    CHECK_OVERFLOW("'mpfr' overflow in "NAME); \
    CHECK_INEXACT("'mpfr' inexact result in "NAME); \
  done: \
    Py_DECREF(self); \
    Py_DECREF(other); \
    if (PyErr_Occurred()) { \
        Py_XDECREF((PyObject*)result); \
        result = NULL; \
    } \
    return (PyObject*)result;

#define MPFR_CLEANUP_RF(NAME) \
    SUBNORMALIZE(rf); \
    MERGE_FLAGS; \
    if ((context->ctx.traps & TRAP_INVALID) && mpfr_nanflag_p()) { \
        GMPY_INVALID("'mpfr' invalid operation in " NAME); \
        Py_DECREF((PyObject*)rf); \
        return NULL; \
    } \
    if ((context->ctx.traps & TRAP_DIVZERO) && mpfr_divby0_p()) { \
        GMPY_DIVZERO("'mpfr' division by zero in " NAME); \
        Py_DECREF((PyObject*)rf); \
        return NULL; \
    } \
    if ((context->ctx.traps & TRAP_UNDERFLOW) && mpfr_underflow_p()) { \
        GMPY_UNDERFLOW("'mpfr' underflow in " NAME); \
        Py_DECREF((PyObject*)rf); \
        return NULL; \
    } \
    if ((context->ctx.traps & TRAP_OVERFLOW) && mpfr_overflow_p()) { \
        GMPY_OVERFLOW("'mpfr' overflow in " NAME); \
        Py_DECREF((PyObject*)rf); \
        return NULL; \
    } \
    if ((context->ctx.traps & TRAP_INEXACT) && mpfr_inexflag_p()) { \
        GMPY_INEXACT("'mpfr' inexact result in " NAME); \
        Py_DECREF((PyObject*)rf); \
        return NULL; \
    } \
    return (PyObject*)rf;

#define MPFR_CLEANUP_RESULT(NAME) \
    SUBNORMALIZE(result); \
    MERGE_FLAGS; \
    if ((context->ctx.traps & TRAP_INVALID) && mpfr_nanflag_p()) { \
        GMPY_INVALID("'mpfr' invalid operation in " NAME); \
        Py_DECREF((PyObject*)result); \
        return NULL; \
    } \
    if ((context->ctx.traps & TRAP_DIVZERO) && mpfr_divby0_p()) { \
        GMPY_DIVZERO("'mpfr' division by zero in " NAME); \
        Py_DECREF((PyObject*)result); \
        return NULL; \
    } \
    if ((context->ctx.traps & TRAP_UNDERFLOW) && mpfr_underflow_p()) { \
        GMPY_UNDERFLOW("'mpfr' underflow in " NAME); \
        Py_DECREF((PyObject*)result); \
        return NULL; \
    } \
    if ((context->ctx.traps & TRAP_OVERFLOW) && mpfr_overflow_p()) { \
        GMPY_OVERFLOW("'mpfr' overflow in " NAME); \
        Py_DECREF((PyObject*)result); \
        return NULL; \
    } \
    if ((context->ctx.traps & TRAP_INEXACT) && mpfr_inexflag_p()) { \
        GMPY_INEXACT("'mpfr' inexact result in " NAME); \
        Py_DECREF((PyObject*)result); \
        return NULL; \
    }

static PyTypeObject MPFR_Type;
#define MPFR(obj) (((MPFR_Object *)(obj))->f)
#define MPFR_Check(v) (((PyObject*)v)->ob_type == &MPFR_Type)

static PyObject * GMPy_MPFR_Factory(PyObject *self, PyObject *args, PyObject *keywds);
static PyObject * Pympfr_getprec_attrib(MPFR_Object *self, void *closure);
static PyObject * Pympfr_getrc_attrib(MPFR_Object *self, void *closure);
static PyObject * Pympfr_getimag_attrib(MPFR_Object *self, void *closure);
static PyObject * Pympfr_getreal_attrib(MPFR_Object *self, void *closure);
static int Pympfr_nonzero(MPFR_Object *self);
static PyObject * Pympfr_conjugate(PyObject *self, PyObject *args);
static PyObject * Pympfr_root(PyObject *self, PyObject *args);
static PyObject * Pympfr_reldiff(PyObject *self, PyObject *args);
static PyObject * Pympfr_ceil(PyObject *self, PyObject *other);
static PyObject * Pympfr_floor(PyObject *self, PyObject *other);
static PyObject * Pympfr_trunc(PyObject *self, PyObject *other);
static PyObject * Pympfr_round2(PyObject *self, PyObject *args);
static PyObject * Pympfr_round10(PyObject *self, PyObject *args);
static PyObject * Pympfr_round_away(PyObject* self, PyObject *other);
static PyObject * Pympfr_modf(PyObject* self, PyObject *other);
static PyObject * Pympfr_lgamma(PyObject* self, PyObject *other);
static PyObject * Pympfr_jn(PyObject* self, PyObject *other);
static PyObject * Pympfr_yn(PyObject* self, PyObject *other);
static PyObject * Pympfr_fmod(PyObject *self, PyObject *args);
static PyObject * Pympfr_remainder(PyObject *self, PyObject *args);
static PyObject * Pympfr_remquo(PyObject* self, PyObject *args);
static PyObject * Pympfr_frexp(PyObject *self, PyObject *other);
static PyObject * Pympfr_agm(PyObject *self, PyObject *args);
static PyObject * Pympfr_max2(PyObject *self, PyObject *args);
static PyObject * Pympfr_min2(PyObject *self, PyObject *args);
static PyObject * Pympfr_nexttoward(PyObject *self, PyObject *args);
static PyObject * Pympfr_nextabove(PyObject *self, PyObject *other);
static PyObject * Pympfr_nextbelow(PyObject *self, PyObject *other);
static PyObject * Pympfr_factorial(PyObject *self, PyObject *other);
static PyObject * Pympfr_is_lessgreater(PyObject *self, PyObject *args);
static PyObject * Pympfr_is_unordered(PyObject *self, PyObject *args);
static PyObject * Pympfr_check_range(PyObject *self, PyObject *other);
static PyObject * Pympfr_fsum(PyObject *self, PyObject *other);

#ifdef __cplusplus
}
#endif
#endif
