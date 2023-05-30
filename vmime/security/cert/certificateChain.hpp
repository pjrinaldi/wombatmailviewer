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

#ifndef VMIME_SECURITY_CERT_CERTIFICATECHAIN_HPP_INCLUDED
#define VMIME_SECURITY_CERT_CERTIFICATECHAIN_HPP_INCLUDED


#include "vmime/types.hpp"

#include "vmime/security/cert/certificate.hpp"


namespace vmime {
namespace security {
namespace cert {


/** An ordered list of certificates, from the subject certificate to
  * the issuer certificate.
  */
class VMIME_EXPORT certificateChain : public object {

public:

	/** Construct a new certificateChain object given an ordered list
	  * of certificates.
	  *
	  * @param certs chain of certificates
	  */
	certificateChain(const std::vector <shared_ptr <certificate> >& certs);

	/** Return the number of certificates in the chain.
	  *
	  * @return number of certificates in the chain
	  */
	size_t getCount() const;

	/** Return the certificate at the specified position. 0 is the
	  * subject certificate, 1 is the issuer's certificate, 2 is
	  * the issuer's issuer, etc.
	  *
	  * @param index position at which to retrieve certificate
	  * @return certificate at the specified position
	  */
	const shared_ptr <certificate>& getAt(const size_t index);

protected:

	std::vector <shared_ptr <certificate> > m_certs;
};


} // cert
} // security
} // vmime


#endif // VMIME_SECURITY_CERT_CERTIFICATECHAIN_HPP_INCLUDED

