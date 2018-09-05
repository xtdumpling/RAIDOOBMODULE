

#ifndef _ISCSI_CHAP_ENCRYPT_H_
#define _ISCSI_CHAP_ENCRYPT_H_

typedef enum {
	EncryptSecrets = 0,
	DecryptSecrets,
	ClearAnEntry,
	ClearAllEntries
}CHAP_ENCRYPTION_OPCODE;
#endif
