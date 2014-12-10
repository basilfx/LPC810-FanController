from libc.stdint cimport int16_t
from libc.stdlib cimport malloc, free

cdef extern from "pid.h":
    ctypedef struct pid_data_t:
        int16_t Kp;
        int16_t Ki;
        int16_t Kd;

    void PID_Controller_Init(pid_data_t *pid, int16_t Kp, int16_t Ki, int16_t Kd);
    int16_t PID_Controller_Update(pid_data_t *pid, int16_t processValue, int16_t setPoint);
    void PID_Controller_Reset(pid_data_t *pid);

cdef class PIDController:
    cdef pid_data_t* _c_pid

    def __cinit__(self):
        self._c_pid = <pid_data_t*>malloc(sizeof(pid_data_t))

        if self._c_pid is NULL:
            raise MemoryError()

    def __dealloc__(self):
        if self._c_pid is not NULL:
            free(self._c_pid)

    def __init__(self, int16_t Kp, int16_t Ki, int16_t Kd):
        PID_Controller_Init(self._c_pid, Kp, Ki, Kd)

    def update(self, int16_t processValue, int16_t setPoint):
        return PID_Controller_Update(self._c_pid, processValue, setPoint)

    def reset(self):
        PID_Controller_Reset(self._c_pid)

    property Kp:
        def __get__(self):
            return self._c_pid.Kp

    property Ki:
        def __get__(self):
            return self._c_pid.Ki

    property Kd:
        def __get__(self):
            return self._c_pid.Kd
