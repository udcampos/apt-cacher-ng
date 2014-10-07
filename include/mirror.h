/*
 * mirror.h
 *
 *  Created on: 25.11.2010
 *      Author: ed
 */

#ifndef MIRROR_H_
#define MIRROR_H_

#include "cacheman.h"

class pkgmirror: public tCacheOperation, ifileprocessor
{
public:
	// XXX: for c++11... using tCacheOperation::tCacheOperation;
	inline pkgmirror(const tSpecialRequest::tRunParms& parms)
	: tCacheOperation(parms) {};

	virtual ~pkgmirror() {};
	void Action() override;

protected:
	// FileHandler
	bool ProcessRegular(const mstring &sPath, const struct stat &);
	virtual void HandlePkgEntry(const tRemoteFileInfo &entry);
	void _LoadKeyCache(const mstring & sFileName);

	bool m_bCalcSize=false, m_bSkipIxUpdate =false,
			m_bDoDownload=false, m_bAsNeeded=false, m_bUseDelta=false;
	off_t m_totalSize=0, m_totalHave=0;
	tStrSet m_pathFilter;

	const tHttpUrl *m_pDeltaSrc=NULL;
	tStrPos m_repCutLen=0;

	bool ConfigDelta(cmstring &sPathRel);
};
#endif /* MIRROR_H_ */
