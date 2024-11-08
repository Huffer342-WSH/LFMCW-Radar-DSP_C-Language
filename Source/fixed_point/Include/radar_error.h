#ifndef _RADAR_ERROR_H_
#define _RADAR_ERROR_H_


#ifdef __cplusplus
extern "C" {
#endif


enum {
    RADAR_SUCCESS = 0,
    RADAR_FAILURE = -1,
    RADAR_CONTINUE = -2, /* iteration has not converged */
    RADAR_EDOM = 1,      /* input domain error, e.g sqrt(-1) */
    RADAR_ERANGE = 2,    /* output range error, e.g. exp(1e100) */
    RADAR_EFAULT = 3,    /* invalid pointer */
    RADAR_EINVAL = 4,    /* invalid argument supplied by user */
    RADAR_EFAILED = 5,   /* generic failure */
    RADAR_EFACTOR = 6,   /* factorization failed */
    RADAR_ESANITY = 7,   /* sanity check failed - shouldn't happen */
    RADAR_ENOMEM = 8,    /* malloc failed */
    RADAR_EBADFUNC = 9,  /* problem with user-supplied function */
    RADAR_ERUNAWAY = 10, /* iterative process is out of control */
    RADAR_EMAXITER = 11, /* exceeded max number of iterations */
    RADAR_EZERODIV = 12, /* tried to divide by zero */
    RADAR_EBADTOL = 13,  /* user specified an invalid tolerance */
    RADAR_ETOL = 14,     /* failed to reach the specified tolerance */
    RADAR_EUNDRFLW = 15, /* underflow */
    RADAR_EOVRFLW = 16,  /* overflow  */
    RADAR_ELOSS = 17,    /* loss of accuracy */
    RADAR_EROUND = 18,   /* failed because of roundoff error */
    RADAR_EBADLEN = 19,  /* matrix, vector lengths are not conformant */
    RADAR_ENOTSQR = 20,  /* matrix not square */
    RADAR_ESING = 21,    /* apparent singularity detected */
    RADAR_EDIVERGE = 22, /* integral or series is divergent */
    RADAR_EUNSUP = 23,   /* requested feature is not supported by the hardware */
    RADAR_EUNIMPL = 24,  /* requested feature not (yet) implemented */
    RADAR_ECACHE = 25,   /* cache limit exceeded */
    RADAR_ETABLE = 26,   /* table limit exceeded */
    RADAR_ENOPROG = 27,  /* iteration is not making progress towards solution */
    RADAR_ENOPROGJ = 28, /* jacobian evaluations are not improving the solution */
    RADAR_ETOLF = 29,    /* cannot reach the specified tolerance in F */
    RADAR_ETOLX = 30,    /* cannot reach the specified tolerance in X */
    RADAR_ETOLG = 31,    /* cannot reach the specified tolerance in gradient */
    RADAR_EOF = 32       /* end of file */
};


#define RADAR_ERROR(reason, error_code)                      \
    do {                                                     \
        radar_error(reason, __FILE__, __LINE__, error_code); \
                                                             \
    } while (0)


void radar_error(const char *reason, const char *file, int line, int error_code);


#ifdef __cplusplus
}
#endif
#endif /* _RADAR_ERROR_H_ */
