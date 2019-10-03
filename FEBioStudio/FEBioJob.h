#pragma once
#include <FSCore/FSObject.h>
#include "LaunchConfig.h"

class CPostDoc;
class CDocument;
#ifdef HAS_SSH
class CSSHHandler;
#endif
class XPLT_OPTIONS;

//-----------------------------------------------------------------------------
class CFEBioJob : public FSObject
{
public:
	enum JOB_STATUS {
		NONE,
		COMPLETED,
		FAILED,
		CANCELLED,
		RUNNING
	};

public:
	CFEBioJob(CDocument* doc);
	~CFEBioJob();
	CFEBioJob(CDocument* doc, const std::string& jobName, const std::string& workingDirectory, CLaunchConfig launchConfig);

	void SetStatus(JOB_STATUS status);
	int GetStatus();

	void SetFileName(const std::string& fileName);
	std::string GetFileName() const;

	void SetPlotFileName(const std::string& plotFile);
	std::string GetPlotFileName() const;

	void UpdateWorkingDirectory(const std::string& dir);

	CLaunchConfig* GetLaunchConfig();
	void UpdateLaunchConfig(CLaunchConfig launchConfig);

#ifdef HAS_SSH
	CSSHHandler* GetSSHHandler();
#endif

	bool OpenPlotFile(const XPLT_OPTIONS& ops);

	bool HasPostDoc();

	CPostDoc* GetPostDoc();

	void Load(IArchive& ar) override;
	void Save(OArchive& ar) override;

protected:
	size_t RemoveChild(FSObject* po) override;

private:
	std::string		m_fileName;	// the .feb file name
	std::string		m_plotFile;	// the .xplt file name
	std::string		m_logFile;	// the .log file name
	int				m_status;	// return status
	CLaunchConfig 	m_launchConfig;
#ifdef HAS_SSH
	CSSHHandler*	m_sshHandler;
	CSSHHandler* 	NewHandler();
#endif

	CDocument*	m_doc;
	CPostDoc*	m_postDoc;

	static int	m_count;
};
