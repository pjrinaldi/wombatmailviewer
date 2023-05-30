//
// VMime library (http://www.vmime.org)
// Copyright (C) 2002 Vincent Richard <vincent@vmime.org>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 3 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// Linking this library statically or dynamically with other modules is making
// a combined work based on this library.  Thus, the terms and conditions of
// the GNU General Public License cover the whole combination.
//

#ifndef VMIME_ADDRESS_HPP_INCLUDED
#define VMIME_ADDRESS_HPP_INCLUDED


#include "vmime/base.hpp"
#include "vmime/headerFieldValue.hpp"


namespace vmime {


/** Abstract class representing a mailbox or a group of mailboxes.
  *
  * This class define a common behaviour for the mailbox
  * and mailboxGroup classes.
  */
class VMIME_EXPORT address : public headerFieldValue {

protected:

	address();

public:

	/** Check whether this address is empty (no mailboxes specified
	  * if this is a mailboxGroup -or- no email specified if this is
	  * a mailbox).
	  *
	  * @return true if this address is empty
	  */
	virtual bool isEmpty() const = 0;

	/** Test whether this is object is a mailboxGroup.
	  *
	  * @return true if this is a mailboxGroup, false otherwise
	  */
	virtual bool isGroup() const = 0;

	virtual shared_ptr <component> clone() const = 0;

	/** Parse an address from an input buffer.
	  *
	  * @param ctx parsing context
	  * @param buffer input buffer
	  * @param position position in the input buffer
	  * @param end end position in the input buffer
	  * @param newPosition will receive the new position in the input buffer
	  * @param isLastAddressOfGroup will be set to true if this is the last address
	  * of a group (end delimiter was found), or false otherwise (may be set to NULL)
	  * @return a new address object, or null if no more address is available in the input buffer
	  */
	static shared_ptr <address> parseNext(
		const parsingContext& ctx,
		const string& buffer,
		const size_t position,
		const size_t end,
		size_t* newPosition,
		const bool allowGroup,
		bool *isLastAddressOfGroup
	);
};


} // vmime


#endif // VMIME_ADDRESS_HPP_INCLUDED
