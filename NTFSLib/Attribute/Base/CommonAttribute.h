#ifndef _NTFSLIB_COMMON_ATTRIBUTE_H
#define _NTFSLIB_COMMON_ATTRIBUTE_H

#include <string>

#include "..\..\NTFSVolume.h"
#include "..\..\Misc\Defs.h"
#include "..\..\Types\NTFSTypes.h"

using std::wstring;

/**
 * Common attribute header wrapper. Contains all the common characteristics for all attributes.
 * Provides basic API for further extension.
 * See: ResidentAttribute, NonResidentAttirbute.
 * We don't save any of the attribute's fields as class fields since we are not interested in
 * evaluating ALL the fields for each attribute we read. Furthermore, in most cases,
 * each getter will be invoked only one time. As a result, we save time and memory.
 */
class CommonAttribute {
public:
	CommonAttribute(NTFSVolume& ntfsVolume, const PCOMMON_ATTR_RECORD attribute);

	virtual ~CommonAttribute();

	/**
	 * Reads <length> bytes of data from a specified <offset>.
	 * If <offset> + <length> is greater than the attribute's size,
	 * an OutOfBoundsError will be thrown. If less than <length> bytes
	 * were read, and BadSizeError will be thrown. <buffer> size must be
	 * equal to or greater than <length>.
	 */
	virtual void getData(PVOID buffer, ULONGLONG offset, DWORD length) = 0;

	/**
	 * Returns the size of the attribute in bytes.
	 */
	virtual ULONGLONG getDataSize() const = 0;

	/**
	 * Returns a pointer to the attribute's data.
	 */
	virtual PBYTE getDataPointer() const = 0;

	/**
	 * Returns a reference to the volume related to this attribute.
	 */
	NTFSVolume& getVolume() const;

	/**
	 * Returns the attribute's name.
	 */
	wstring getAttributeName();

	/**
	 * Returns the attribute's instance ID.
	 * The attribute's instance ID is a unique number attached to each attribute
	 * in a single MFT record.
	 */
	WORD getAttributeInstance() const;

	/**
	 * Returns the attribute's type.
	 */
	DWORD getAttributeType() const;

	/**
	 * Returns the total size of the resident part of the attribute, including the header.
	 */
	DWORD getAttirbuteSize() const;

	/**
	 * Returns true of the attribute is resident, false otherwise.
	 */
	bool isResident() const;

	/**
	 * Returns true if the attribute is encrypted, false otherwise.
	 */
	bool isEncrypted() const;

	/**
	 * Returns true if the attribute is compressed, false otherwise.
	 */
	bool isCompressed() const;

	/**
	 * Returns true of the attribute is sparse (filled with 0's), false otherwise.
	 */
	bool isSparse() const;

protected:
	// Actual attribute header.
	const PCOMMON_ATTR_RECORD m_attribute;

	// Reference to the related NTFS volume.
	NTFSVolume& m_ntfsVolume;

private:
	FORBID_COPY_AND_ASSIGN(CommonAttribute);
};

#endif // _NTFSLIB_COMMON_ATTRIBUTE_H
