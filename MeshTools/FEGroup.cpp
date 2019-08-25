// FEGroup.cpp: implementation of the FEGroup class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FEGroup.h"
#include <MeshLib/FEMesh.h>
#include "FEModel.h"
#include <GeomLib/GObject.h>

//////////////////////////////////////////////////////////////////////
// FEGroup
//////////////////////////////////////////////////////////////////////

FEGroup::FEGroup(GObject* po, int ntype) : FEItemListBuilder(ntype)
{
	// store a ptr to the mesh this group belongs to
	m_pObj = po;
	if (po) m_objID = po->GetID(); else m_objID = -1;
}

void FEGroup::SetGObject(GObject* po)
{
	m_pObj = po; 
	if (po) m_objID = po->GetID(); else m_objID = -1;
}

GObject* FEGroup::GetGObject()
{
	return m_pObj;
}

FEMesh* FEGroup::GetMesh()
{
	return m_pObj->GetFEMesh();
}

FEGroup::~FEGroup()
{

}

void FEGroup::Save(OArchive& ar)
{
	int meshid = m_pObj->GetID();
	int N = (int)m_Item.size();
	ar.WriteChunk(ID, m_nID);
	ar.WriteChunk(NAME, GetName());
	ar.WriteChunk(MESHID, meshid);
	ar.WriteChunk(SIZE, N);

	list<int>::iterator it = m_Item.begin();
	for (int i=0; i<N; ++i, ++it)
	{
		ar.WriteChunk(ITEM, (*it));
	}
}

//-----------------------------------------------------------------------------
// Note that this function only reads the meshID. If this group is not managed
// by an FEMesh, then the owner of this group is responsible for setting the 
// correct mesh pointer (i.e. m_pMesh).
void FEGroup::Load(IArchive &ar)
{
	TRACE("FEGroup::Load");

	m_Item.clear();

	int N, n;
	while (IArchive::IO_OK == ar.OpenChunk())
	{
		int nid = ar.GetChunkID();
		switch (nid)
		{
		case ID: ar.read(n); SetID(n); break;
		case NAME: { char sz[256]; ar.read(sz); SetName(sz); } break;
		case MESHID: ar.read(m_objID); break;
/*			{
				PRVArchive& prv = dynamic_cast<PRVArchive&>(ar);
				FEModel& fem = *prv.GetFEModel();
				int meshid;
				ar.read(meshid);
				if (meshid != -1)
				{
					GObject* po = fem.GetModel().FindObject(meshid);
					if (po == 0) throw ReadError("Invalid mesh ID in FEGroup::Load");
					SetMesh(po->GetFEMesh());
				}
			}
			break;
*/		case SIZE: ar.read(N); break;
		case ITEM: ar.read(n); m_Item.push_back(n); break;
		default:
			throw ReadError("unknown CID in FEGroup::Load");
		}
		ar.CloseChunk();
	}
	assert((int) m_Item.size() == N);
}

//////////////////////////////////////////////////////////////////////
// FEPart
//////////////////////////////////////////////////////////////////////

FEPart::FEPart(GObject* po, const vector<int>& elset) : FEGroup(po, FE_PART)
{
	int nsel = (int) elset.size();
	if (nsel > 0)
	{
		for (int i=0; i<nsel; ++i) add(elset[i]);
	}
}

void FEPart::Copy(FEPart* pg)
{
	m_Item = pg->m_Item;
	SetName(pg->GetName());
}

FEItemListBuilder* FEPart::Copy()
{
	FEPart* pg = new FEPart(m_pObj);
	pg->m_Item = m_Item;
	return pg;
}

//-----------------------------------------------------------------------------
FEElemList* FEPart::BuildElemList()
{
	FEMesh* pm = m_pObj->GetFEMesh();
	if (pm==0) return 0;

	FEElemList* pg = new FEElemList();

	list<int>::iterator it;
	for (it = m_Item.begin(); it != m_Item.end(); ++it) pg->Add(pm, pm->ElementPtr(*it));

	return pg;
}

//-----------------------------------------------------------------------------
FENodeList* FEPart::BuildNodeList()
{
	int i, j;
	FEMesh* pm = m_pObj->GetFEMesh();
	int N = pm->Nodes();
	for (i=0; i<N; ++i) pm->Node(i).m_ntag = 0;

	list<int>::iterator it;
	for (it = m_Item.begin(); it != m_Item.end(); ++it)
	{
		FEElement* pe = pm->ElementPtr(*it);
		for (j=0; j<pe->Nodes(); ++j) pm->Node(pe->m_node[j]).m_ntag = 1;
	}

	FENodeList* pg = new FENodeList();
	for (i=0; i<N; ++i)
	{
		FENode& n = pm->Node(i);
		if (n.m_ntag == 1) pg->Add(pm, &n);
	}
	return pg;
}

//////////////////////////////////////////////////////////////////////
// FESurface
//////////////////////////////////////////////////////////////////////

FESurface::FESurface(GObject* po, vector<int>& face) : FEGroup(po, FE_SURFACE)
{
	int n = (int) face.size();
	if (n > 0)
	{
		for (int i=0; i<n; ++i) add(face[i]);
	}
}

void FESurface::Copy(FESurface* pg)
{
	m_Item = pg->m_Item;
	SetName(pg->GetName());
}

FEItemListBuilder* FESurface::Copy()
{
	FESurface* pg = new FESurface(m_pObj);
	pg->m_Item = m_Item;
	return pg;
}

FEFaceList* FESurface::BuildFaceList()
{
	FEMesh* pm = m_pObj->GetFEMesh();
	if (pm == 0) return 0;

	FEFaceList* ps = new FEFaceList();

	list<int>::iterator it = m_Item.begin();

	for (int i=0; i<size(); ++i, ++it) ps->Add(pm, pm->FacePtr(*it));
	return ps;
}

FENodeList* FESurface::BuildNodeList()
{	
	FEMesh* pm = m_pObj->GetFEMesh();
	if (pm == 0) return 0;

	FENodeList* pg = new FENodeList();
	
	// tag all nodes to be added
	int i, j, n;
	for (i=0; i<pm->Nodes(); ++i) pm->Node(i).m_ntag = 0;

	list<int>::iterator it = m_Item.begin();
	int N = (int)m_Item.size();
	for (i=0; i<N; ++i, ++it)
	{
		FEFace& f = pm->Face(*it);
		n = f.Nodes();
		for (j=0; j<n; ++j) pm->Node(f.n[j]).m_ntag = 1;
	}

	// add nodes to list
	for (i=0; i<pm->Nodes(); ++i)
	{
		FENode* pn = pm->NodePtr(i);
		if (pn->m_ntag == 1) pg->Add(pm, pn);
	}

	return pg;
}


//////////////////////////////////////////////////////////////////////
// FEEdgeSet
//////////////////////////////////////////////////////////////////////

FEEdgeSet::FEEdgeSet(GObject* po, vector<int>& edge) : FEGroup(po, FE_EDGESET)
{
	int n = (int) edge.size();
	if (n > 0)
	{
		for (int i=0; i<n; ++i) add(edge[i]);
	}
}

void FEEdgeSet::Copy(FEEdgeSet* pg)
{
	m_Item = pg->m_Item;
	SetName(pg->GetName());
}

FEItemListBuilder* FEEdgeSet::Copy()
{
	FEEdgeSet* pg = new FEEdgeSet(m_pObj);
	pg->m_Item = m_Item;
	return pg;
}

FENodeList* FEEdgeSet::BuildNodeList()
{	
	FEMesh* pm = m_pObj->GetFEMesh();
	if (pm == 0) return 0;

	FENodeList* pg = new FENodeList();
	
	// tag all nodes to be added
	int i, j, n;
	for (i=0; i<pm->Nodes(); ++i) pm->Node(i).m_ntag = 0;

	list<int>::iterator it = m_Item.begin();
	int N = (int)m_Item.size();
	for (i=0; i<N; ++i, ++it)
	{
		FEEdge& e = pm->Edge(*it);
		n = e.Nodes();
		for (j=0; j<n; ++j) pm->Node(e.n[j]).m_ntag = 1;
	}

	// add nodes to list
	for (i=0; i<pm->Nodes(); ++i)
	{
		FENode* pn = pm->NodePtr(i);
		if (pn->m_ntag == 1) pg->Add(pm, pn);
	}

	return pg;
}

//////////////////////////////////////////////////////////////////////
// FENodeSet
//////////////////////////////////////////////////////////////////////

FENodeSet::FENodeSet(GObject* po, const vector<int>& node) : FEGroup(po, FE_NODESET)
{
	int n = (int) node.size();
	if (n > 0)
	{
		for (int i=0; i<n; ++i) add(node[i]);
	}
}

void FENodeSet::Copy(FENodeSet* pg)
{
	m_Item = pg->m_Item;
	SetName(pg->GetName());
}

FEItemListBuilder* FENodeSet::Copy()
{
	FENodeSet* pg = new FENodeSet(m_pObj);
	pg->m_Item = m_Item;
	return pg;
}

FENodeList* FENodeSet::BuildNodeList()
{
	FEMesh* pm = m_pObj->GetFEMesh();
	if (pm == 0) return 0;
	FENodeList* ps = new FENodeList();
	list<int>::iterator it = m_Item.begin();
	for (int i=0; i<size(); ++i, ++it) ps->Add(pm, pm->NodePtr(*it));
	return ps;
}
