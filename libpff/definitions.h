/*
 * Definitions for libpff
 *
 * Copyright (C) 2008-2021, Joachim Metz <joachim.metz@gmail.com>
 *
 * Refer to AUTHORS for acknowledgements.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#if !defined( _LIBPFF_DEFINITIONS_H )
#define _LIBPFF_DEFINITIONS_H

#include "libpff/types.h"

#define LIBPFF_VERSION					20211114

/* The version string
 */
#define LIBPFF_VERSION_STRING				"20211114"

/* The access flags definitions
 * bit 1        set to 1 for read access
 * bit 2        set to 1 for write access
 * bit 3-8      not used
 */
enum LIBPFF_ACCESS_FLAGS
{
	LIBPFF_ACCESS_FLAG_READ				= 0x01,
/* Reserved: not supported yet */
	LIBPFF_ACCESS_FLAG_WRITE			= 0x02
};

/* The file access macros
 */
#define LIBPFF_OPEN_READ				( LIBPFF_ACCESS_FLAG_READ )
/* Reserved: not supported yet */
#define LIBPFF_OPEN_WRITE				( LIBPFF_ACCESS_FLAG_WRITE )
/* Reserved: not supported yet */
#define LIBPFF_OPEN_READ_WRITE				( LIBPFF_ACCESS_FLAG_READ | LIBPFF_ACCESS_FLAG_WRITE )

/* The recovery flags
 */
enum LIBPFF_RECOVERY_FLAGS
{
	LIBPFF_RECOVERY_FLAG_IGNORE_ALLOCATION_DATA	= 0x01,
	LIBPFF_RECOVERY_FLAG_SCAN_FOR_FRAGMENTS		= 0x02
};

/* The file types
 */
enum LIBPFF_FILE_TYPES
{
	LIBPFF_FILE_TYPE_32BIT				= 32,
	LIBPFF_FILE_TYPE_64BIT				= 64,
	LIBPFF_FILE_TYPE_64BIT_4K_PAGE			= 65
};

/* The file content types
 */
enum LIBPFF_FILE_CONTENT_TYPES
{
	LIBPFF_FILE_CONTENT_TYPE_PAB			= (int) 'a',
	LIBPFF_FILE_CONTENT_TYPE_PST			= (int) 'p',
	LIBPFF_FILE_CONTENT_TYPE_OST			= (int) 'o'
};

/* The encryption types
 */
enum LIBPFF_ENCRYPTION_TYPES
{
	LIBPFF_ENCRYPTION_TYPE_NONE			= 0,
	LIBPFF_ENCRYPTION_TYPE_COMPRESSIBLE		= 1,
	LIBPFF_ENCRYPTION_TYPE_HIGH			= 2
};

/* The item types
 * These item types partially map to the message classes
 * LIBPFF_ITEM_TYPE_UNDEFINED				(initialization value)
 * LIBPFF_ITEM_TYPE_ACTIVITY				IPM.Activity
 * LIBPFF_ITEM_TYPE_APPOINTMENT				IPM.Appointment
 * LIBPFF_ITEM_TYPE_ATTACHMENT				(attachment)
 * LIBPFF_ITEM_TYPE_ATTACHMENTS				(attachments)
 * LIBPFF_ITEM_TYPE_COMMON				IPM
 * LIBPFF_ITEM_TYPE_CONFIGURATION			IPM.Configuration.*
 * LIBPFF_ITEM_TYPE_CONFLICT_MESSAGE			IPM.Conflict.Message
 * LIBPFF_ITEM_TYPE_CONTACT				IPM.Contact
 * LIBPFF_ITEM_TYPE_DISTRIBUTION_LIST			IPM.DistList
 * LIBPFF_ITEM_TYPE_DOCUMENT				IPM.Document.*
 * LIBPFF_ITEM_TYPE_EMAIL				IPM.Note, REPORT.IPM.Note.*
 * LIBPFF_ITEM_TYPE_EMAIL_SMIME				IPM.Note.SMIME.*
 * LIBPFF_ITEM_TYPE_FAX					IPM.FAX, IPM.Note.Fax
 * LIBPFF_ITEM_TYPE_FOLDER				(folder/container)
 * LIBPFF_ITEM_TYPE_MEETING				IPM.Schedule.Meeting
 * LIBPFF_ITEM_TYPE_MMS					IPM.Note.Mobile.MMS
 * LIBPFF_ITEM_TYPE_NOTE				IPM.StickyNote
 * LIBPFF_ITEM_TYPE_POSTING_NOTE			IPM.Post
 * LIBPFF_ITEM_TYPE_RECIPIENTS				(recipients)
 * LIBPFF_ITEM_TYPE_RSS_FEED				IPM.Post.RSS
 * LIBPFF_ITEM_TYPE_SHARING				IPM.Sharing.*
 * LIBPFF_ITEM_TYPE_SMS					IPM.Note.Mobile.SMS
 * LIBPFF_ITEM_TYPE_TASK				IPM.Task
 * LIBPFF_ITEM_TYPE_TASK_REQUEST			IPM.TaskRequest.*
 * LIBPFF_ITEM_TYPE_VOICEMAIL				IPM.Note.Voicemail
 * LIBPFF_ITEM_TYPE_UNKNOWN				(unknown item type, used in folder content type)
 *
 * Unsupported:
 * IPM.Post
 */
enum LIBPFF_ITEM_TYPES
{
	LIBPFF_ITEM_TYPE_UNDEFINED,
	LIBPFF_ITEM_TYPE_ACTIVITY,
	LIBPFF_ITEM_TYPE_APPOINTMENT,
	LIBPFF_ITEM_TYPE_ATTACHMENT,
	LIBPFF_ITEM_TYPE_ATTACHMENTS,
	LIBPFF_ITEM_TYPE_COMMON,
	LIBPFF_ITEM_TYPE_CONFIGURATION,
	LIBPFF_ITEM_TYPE_CONFLICT_MESSAGE,
	LIBPFF_ITEM_TYPE_CONTACT,
	LIBPFF_ITEM_TYPE_DISTRIBUTION_LIST,
	LIBPFF_ITEM_TYPE_DOCUMENT,
	LIBPFF_ITEM_TYPE_EMAIL,
	LIBPFF_ITEM_TYPE_EMAIL_SMIME,
	LIBPFF_ITEM_TYPE_FAX,
	LIBPFF_ITEM_TYPE_FOLDER,
	LIBPFF_ITEM_TYPE_MEETING,
	LIBPFF_ITEM_TYPE_MMS,
	LIBPFF_ITEM_TYPE_NOTE,
	LIBPFF_ITEM_TYPE_POSTING_NOTE,
	LIBPFF_ITEM_TYPE_RECIPIENTS,
	LIBPFF_ITEM_TYPE_RSS_FEED,
	LIBPFF_ITEM_TYPE_SHARING,
	LIBPFF_ITEM_TYPE_SMS,
	LIBPFF_ITEM_TYPE_SUB_ASSOCIATED_CONTENTS,
	LIBPFF_ITEM_TYPE_SUB_FOLDERS,
	LIBPFF_ITEM_TYPE_SUB_MESSAGES,
	LIBPFF_ITEM_TYPE_TASK,
	LIBPFF_ITEM_TYPE_TASK_REQUEST,
	LIBPFF_ITEM_TYPE_VOICEMAIL,
	LIBPFF_ITEM_TYPE_UNKNOWN
};

/* The attachment types
 */
enum LIBPFF_ATTACHMENT_TYPES
{
	LIBPFF_ATTACHMENT_TYPE_UNDEFINED		= 0,
	LIBPFF_ATTACHMENT_TYPE_DATA			= (int) 'd',
	LIBPFF_ATTACHMENT_TYPE_ITEM			= (int) 'i',
	LIBPFF_ATTACHMENT_TYPE_REFERENCE		= (int) 'r'
};

/* The unallocated block type
 */
enum LIBPFF_UNALLOCATED_BLOCK_TYPES
{
	LIBPFF_UNALLOCATED_BLOCK_TYPE_DATA		= (int) 'd',
	LIBPFF_UNALLOCATED_BLOCK_TYPE_PAGE		= (int) 'p'
};

/* The name to id map entry types
 */
enum LIBPFF_NAME_TO_ID_MAP_ENTRY_TYPES
{
	LIBPFF_NAME_TO_ID_MAP_ENTRY_TYPE_NUMERIC	= (int) 'n',
	LIBPFF_NAME_TO_ID_MAP_ENTRY_TYPE_STRING		= (int) 's'
};

/* The entry value flags
 */
enum LIBPFF_ENTRY_VALUE_FLAGS
{
	LIBPFF_ENTRY_VALUE_FLAG_MATCH_ANY_VALUE_TYPE	= 0x01,
	LIBPFF_ENTRY_VALUE_FLAG_IGNORE_NAME_TO_ID_MAP	= 0x02
};

#endif /* !defined( _LIBPFF_DEFINITIONS_H ) */

