#pragma once
#include <MeshIO/FileReader.h>
#include <MeshTools/FEProject.h>
#include "AbaqusModel.h"

#include <list>
using namespace std;

//-----------------------------------------------------------------------------
// Implements a class to import ABAQUS files
// 
class AbaqusImport : public FEFileImport
{
	// attributes
	struct ATTRIBUTE
	{
		char szatt[AbaqusModel::Max_Name];	// name of attribute
		char szval[AbaqusModel::Max_Name];	// value of attribute
	};

public:
	class Exception{};

public:	// import options
	bool	m_bnodesets;	// read node sets
	bool	m_belemsets;	// read element sets
	bool	m_bfacesets;	// read the surfaces
	bool	m_bautopart;	// auto-partition parts
	bool	m_bautosurf;	// auto-partition surfaces
	bool	m_breadPhysics;	// read the physics (i.e. materials, bcs, etc).

public:
	AbaqusImport();
	virtual ~AbaqusImport();

	bool Load(FEProject& fem, const char* szfile);

protected:
	// read a line and increment line counter
	bool read_line(char* szline, FILE* fp);

	// build the model
	bool build_model();

	// build a mesh
	bool build_mesh();

	// build all physics
	bool build_physics();

	// build a part
	GObject* build_part(AbaqusModel::PART* pg);

	// build a surface
	FESurface* build_surface(AbaqusModel::SURFACE* ps);

	// build a nodeset
	FENodeSet* build_nodeset(AbaqusModel::NODE_SET* ns);

	// Keyword parsers
	bool read_heading            (char* szline, FILE* fp);
	bool read_nodes              (char* szline, FILE* fp);
	bool read_ngen               (char* szline, FILE* fp);
	bool read_nfill              (char* szline, FILE* fp);
	bool read_elements           (char* szline, FILE* fp);
	bool read_element_sets       (char* szline, FILE* fp);
	bool read_node_sets          (char* szline, FILE* fp);
	bool read_surface            (char* szline, FILE* fp);
	bool read_surface_interaction(char* szline, FILE* fp);
	bool read_materials          (char* szline, FILE* fp);
	bool read_part               (char* szline, FILE* fp);
	bool read_end_part           (char* szline, FILE* fp);
	bool read_instance           (char* szline, FILE* fp);
	bool read_end_instance       (char* szline, FILE* fp);
	bool read_assembly           (char* szline, FILE* fp);
	bool read_end_assembly       (char* szline, FILE* fp);
	bool read_spring_elements    (char* szline, FILE* fp);
	bool read_step				 (char* szline, FILE* fp);
	bool read_boundary           (char* szline, FILE* fp);
	bool read_dsload             (char* szline, FILE* fp);
	bool read_solid_section      (char* szline, FILE* fp);
	bool read_static             (char* szline, FILE* fp);
	bool read_orientation        (char* szline, FILE* fp);
	bool read_distribution       (char* szline, FILE* fp);

	// skip until we find the next keyword
	bool skip_keyword(char* szline, FILE* fp);

protected:
	// parse a file for keywords
	bool parse_file(FILE* fp);

	// parse the line for attributes
	int parse_line(const char* szline, ATTRIBUTE* pa);

	// find an attribute in a list
	const char* find_attribute(ATTRIBUTE* pa, int nmax, const char* szatt);

private:
	char		m_szTitle[AbaqusModel::Max_Title + 1];
	FEProject*	m_pprj;

	FEModel*	m_pfem;

	AbaqusModel		m_inp;

	int	m_nline;	// current line number
};