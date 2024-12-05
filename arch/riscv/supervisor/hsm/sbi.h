#ifndef __SBI_H__
#define __SBI_H__

enum sbi_ret_error {
	SBI_SUCCESS = 0,
	SBI_ERR_FAILED = -1,
	SBI_ERR_NOT_SUPPORTED = -2,
	SBI_ERR_INVALID_PARAM = -3,
	SBI_ERR_DENIED = -4,
	SBI_ERR_INVALID_ADDRESS = -5,
	SBI_ERR_ALREADY_AVAILABLE = -6,
	SBI_ERR_ALREADY_STARTED = -7,
	SBI_ERR_ALREADY_STOPPED = -8,
	SBI_ERR_NO_SHMEM = -9,
};

enum sbi_ext {
	HSM_EXT = 0x0048534D,
	DBCN_EXT = 0x4442434E,
	SRST_EXT = 0x53525354,
};

enum hsm_actions {
	HSM_HART_START = 0x00,
	HSM_HART_STOP = 0x01,
	HSM_HART_GET_STATUS = 0x02,
	HSM_HART_SUSPEND = 0x03,
};

enum hsm_states {
	HSM_HART_STARTED = 0,
	HSM_HART_STOPPED = 1,
	HSM_HART_START_PENDING = 2,
	HSM_HART_STOP_PENDING = 3,
	HSM_HART_SUSPENDED = 4,
	HSM_HART_SUSPEND_PENDING = 5,
	HSM_HART_RESUME_PENDING = 6,
};

enum dbcn_actions {
	DBCN_WRITE = 0x00,
};

enum srst_actions {
	SRST_RESET = 0,
};

enum srst_type {
	SRST_SHUTDOWN = 0,
	SRST_COLD_REBOOT = 1,
	SRST_WARM_REBOOT = 2,
};

enum srst_reason {
	SRST_NO_REASON = 0,
	SRST_SYS_FAILURE = 1,
};

struct sbi_ret {
	long error;
	long value;
};

struct sbi_ret __sbi_ecall(unsigned long arg0, unsigned long arg1, unsigned long arg2,
						   unsigned long arg3, unsigned long arg4, unsigned long arg5, int fid,
						   int ext);

#define sbi_ecall0(ext, fid) __sbi_ecall(0, 0, 0, 0, 0, 0, fid, ext)
#define sbi_ecall1(ext, fid, arg0) __sbi_ecall(arg0, 0, 0, 0, 0, 0, fid, ext)
#define sbi_ecall2(ext, fid, arg0, arg1) __sbi_ecall(arg0, arg1, 0, 0, 0, 0, fid, ext)
#define sbi_ecall3(ext, fid, arg0, arg1, arg2) __sbi_ecall(arg0, arg1, arg2, 0, 0, 0, fid, ext)

#endif // __SBI_H__
