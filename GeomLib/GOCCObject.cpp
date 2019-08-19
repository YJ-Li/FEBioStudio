#include "GOCCObject.h"
#include <MeshTools/NetGenMesher.h>

#ifdef HAS_OCC
#include <gp_Pnt.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <GC_MakeSegment.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepTools.hxx>
#endif

#ifdef HAS_OCC
class OCC_Data
{
public:
	OCC_Data(){}

public:
	TopoDS_Shape	m_shape;
};

#else
class OCC_Data {
public:
	OCC_Data(){}
};
#endif

GOCCObject::GOCCObject(int type) : GObject(type)
{
	m_occ = new OCC_Data;
	m_pMesher = new NetGenMesher(this);
}

void GOCCObject::SetShape(TopoDS_Shape& shape, bool bupdate)
{
#ifdef HAS_OCC
	m_occ->m_shape = shape;
	if (bupdate)
	{
		BuildGObject();
		BuildGMesh();
	}
#endif
}

TopoDS_Shape& GOCCObject::GetShape()
{
#ifdef HAS_OCC
	return m_occ->m_shape;
#else
	TopoDS_Shape* dummy;
	return *dummy;
#endif
}

void GOCCObject::BuildGObject()
{
	ClearAll();
#ifdef HAS_OCC
	// build nodes
	for (TopExp_Explorer anExpSF(m_occ->m_shape, TopAbs_VERTEX); anExpSF.More(); anExpSF.Next())
	{
		const TopoDS_Vertex& vertex = TopoDS::Vertex(anExpSF.Current());
		gp_Pnt p = BRep_Tool::Pnt(vertex);
		AddNode(vec3d(p.X(), p.Y(), p.Z()));
	}

	// build edges
	// NOTE: It looks like OCC is counting edges on each face. (so for a box, we would have 24 edges, instead of 12)
	int iedge = 0;
	for (TopExp_Explorer anExpSF(m_occ->m_shape, TopAbs_EDGE); anExpSF.More(); anExpSF.Next(), iedge++)
	{
//		TopoDS_Edge edge = TopoDS::Edge(anExpSF.Current());
		GEdge* edge = new GEdge(this);
		edge->SetID(GEdge::CreateUniqueID());
		edge->SetLocalID(iedge);
		edge->m_node[0] = -1;
		edge->m_node[1] = -1;
		AddEdge(edge);
	}

	// build faces
	int iface = 0;
	for (TopExp_Explorer anExpSF(m_occ->m_shape, TopAbs_FACE); anExpSF.More(); anExpSF.Next(), iface++)
	{
		GFace* face = new GFace(this);
		face->SetID(GFace::CreateUniqueID());
		face->SetLocalID(iface);
		face->m_nPID[0] = 0;
		AddFace(face);
	}

	// add a part
	AddPart();
#endif
}

void GOCCObject::BuildGMesh()
{
#ifdef HAS_OCC
	// Generate a mesh
	BRepMesh_IncrementalMesh aMesh(m_occ->m_shape, 0.1, true, 0.25);

	// count the nodes and triangles
	Standard_Integer aNbNodes = 0;
	Standard_Integer aNbTriangles = 0;

	// calculate total number of the nodes and triangles
	for (TopExp_Explorer anExpSF(m_occ->m_shape, TopAbs_FACE); anExpSF.More(); anExpSF.Next())
	{
		TopLoc_Location aLoc;
		Handle(Poly_Triangulation) aTriangulation = BRep_Tool::Triangulation(TopoDS::Face(anExpSF.Current()), aLoc);
		if (!aTriangulation.IsNull())
		{
			aNbNodes += aTriangulation->NbNodes();
			aNbTriangles += aTriangulation->NbTriangles();
		}
	}

	// count edges
	int aNbEdges = 0;
	for (TopExp_Explorer anExpSF(m_occ->m_shape, TopAbs_EDGE); anExpSF.More(); anExpSF.Next())
	{
		const TopoDS_Edge& aEdge = TopoDS::Edge(anExpSF.Current());

		//try to find PolygonOnTriangulation
		Handle(Poly_PolygonOnTriangulation) aPT;
		Handle(Poly_Triangulation) aT;
		TopLoc_Location aL;
		BRep_Tool::PolygonOnTriangulation(aEdge, aPT, aT, aL);

		Standard_Integer nbNodes = aPT->NbNodes();
		const TColStd_Array1OfInteger& nodeList = aPT->Nodes();
		for (Standard_Integer j = 1; j < nbNodes; j++, aNbEdges++) {
		}
	}

	// create a new GMesh object
	m_pGMesh = new GLMesh;
	m_pGMesh->Create(aNbNodes, aNbTriangles, aNbEdges);

	Standard_Integer aNodeOffset = 0;
	Standard_Integer aTriangleOffet = 0;
	int edges = 0, edgeID = 0;
	int faceId = 0;
	for (TopExp_Explorer anExpSF(m_occ->m_shape, TopAbs_FACE); anExpSF.More(); anExpSF.Next(), faceId++)
	{
		TopoDS_Face face = TopoDS::Face(anExpSF.Current());

		TopLoc_Location aLoc;
		Handle(Poly_Triangulation) aTriangulation = BRep_Tool::Triangulation(face, aLoc);

		const TColgp_Array1OfPnt& aNodes = aTriangulation->Nodes();
		const Poly_Array1OfTriangle& aTriangles = aTriangulation->Triangles();

		// copy nodes
		gp_Trsf aTrsf = aLoc.Transformation();
		for (Standard_Integer aNodeIter = aNodes.Lower(); aNodeIter <= aNodes.Upper(); ++aNodeIter)
		{
			GMesh::NODE& node = m_pGMesh->Node(aNodeIter + aNodeOffset - 1);
			gp_Pnt aPnt = aNodes(aNodeIter);
			aPnt.Transform(aTrsf);
			node.r = vec3d(aPnt.X(), aPnt.Y(), aPnt.Z());
		}

		// copy triangles
		const TopAbs_Orientation anOrientation = anExpSF.Current().Orientation();
		for (Standard_Integer aTriIter = aTriangles.Lower(); aTriIter <= aTriangles.Upper(); ++aTriIter)
		{
			Poly_Triangle aTri = aTriangles(aTriIter);

			Standard_Integer anId[3];
			aTri.Get(anId[0], anId[1], anId[2]);
			if (anOrientation == TopAbs_REVERSED)
			{
				// Swap 1, 2.
				Standard_Integer aTmpIdx = anId[1];
				anId[1] = anId[2];
				anId[2] = aTmpIdx;
			}

			GMesh::FACE& face = m_pGMesh->Face(aTriIter + aTriangleOffet - 1);
			face.n[0] = anId[0] + aNodeOffset-1;
			face.n[1] = anId[1] + aNodeOffset-1;
			face.n[2] = anId[2] + aNodeOffset-1;
			face.pid = faceId;
			face.sid = faceId;
		}

		for (TopExp_Explorer edgeExp(face, TopAbs_EDGE); edgeExp.More(); edgeExp.Next())
		{
			const TopoDS_Edge& aEdge = TopoDS::Edge(edgeExp.Current());

			//try to find PolygonOnTriangulation
			Handle(Poly_PolygonOnTriangulation) aPT;
			Handle(Poly_Triangulation) aT;
			TopLoc_Location aL;
			aPT = BRep_Tool::PolygonOnTriangulation(aEdge, aTriangulation, aL);

			if (aPT.IsNull() == false)
			{
				Standard_Integer nbNodes = aPT->NbNodes();
				const TColStd_Array1OfInteger& nodeList = aPT->Nodes();
				for (Standard_Integer j = 1; j < nbNodes; j++, edges++) {
					int inode0 = nodeList.Value(j);
					int inode1 = nodeList.Value(j + 1);

					GMesh::EDGE& edge = m_pGMesh->Edge(edges);
					edge.n[0] = inode0 - 1 + aNodeOffset;
					edge.n[1] = inode1 - 1 + aNodeOffset;
					edge.pid = edgeID++;
				}
			}
		}

		aNodeOffset += aNodes.Size();
		aTriangleOffet += aTriangles.Size();
	}

	// update the GMesh
	m_pGMesh->Update();
	m_pGMesh->UpdateNormals();

#endif // HAS_OCC
}

void GOCCObject::Save(OArchive& ar)
{
	ar.BeginChunk(0);
	{
		GObject::Save(ar);
	}
	ar.EndChunk();

	std::stringstream ss;
	BRepTools::Write(m_occ->m_shape, ss);
	ar.WriteChunk(1, ss.str());
}

void GOCCObject::Load(IArchive& ar)
{
	while (IO_OK == ar.OpenChunk())
	{
		int nid = ar.GetChunkID();
		switch (nid)
		{
		case 0: GObject::Load(ar); break;
		case 1:
		{
			std::string s;
			ar.read(s);
			std::stringstream ss(s);
			BRep_Builder aBuilder;
			BRepTools::Read(m_occ->m_shape, ss, aBuilder);
			BuildGMesh();
		}
		break;
		}
		ar.CloseChunk();
	}
}

//===================================================================
GOCCBottle::GOCCBottle() : GOCCObject(GOCC_BOTTLE)
{
	AddDoubleParam(1.0, "width");
	AddDoubleParam(1.5, "height");
	AddDoubleParam(0.5, "thickness");
}

bool GOCCBottle::Update(bool b)
{
#ifdef HAS_OCC

	double h = GetFloatValue(HEIGHT);
	double w = GetFloatValue(WIDTH);
	double t = GetFloatValue(THICKNESS);
	
	if (w <= 0.0) return false;
	if (h <= 0.0) return false;
	if (t <= 0.0) return false;
	
	// make the OCC object
	MakeBottle(h,w,t);

	// build the GObject structure
	BuildGObject();

	// build the viz mesh
	BuildGMesh();

	return true;
#else
	return false;
#endif // HAS_OCC
}

void GOCCBottle::MakeBottle(double h, double w, double t)
{
#ifdef HAS_OCC
	gp_Pnt aPnt1(-w / 2., 0, 0);
	gp_Pnt aPnt2(-w / 2., -t / 4., 0);
	gp_Pnt aPnt3(0, -t / 2., 0);
	gp_Pnt aPnt4(w / 2., -t / 4., 0);
	gp_Pnt aPnt5(w / 2., 0, 0);

	Standard_Real xValue1 = aPnt1.X();

	Handle(Geom_TrimmedCurve) aArcOfCircle = GC_MakeArcOfCircle(aPnt2, aPnt3, aPnt4);
	Handle(Geom_TrimmedCurve) aSegment1 = GC_MakeSegment(aPnt1, aPnt2);
	Handle(Geom_TrimmedCurve) aSegment2 = GC_MakeSegment(aPnt4, aPnt5);

	TopoDS_Edge aEdge1 = BRepBuilderAPI_MakeEdge(aSegment1);
	TopoDS_Edge aEdge2 = BRepBuilderAPI_MakeEdge(aArcOfCircle);
	TopoDS_Edge aEdge3 = BRepBuilderAPI_MakeEdge(aSegment2);


	TopoDS_Wire aWire = BRepBuilderAPI_MakeWire(aEdge1, aEdge2, aEdge3);

	const gp_Ax1& xAxis = gp::OX();

	gp_Trsf aTrsf;
	aTrsf.SetMirror(xAxis);

	BRepBuilderAPI_Transform aBRepTrsf(aWire, aTrsf);

	TopoDS_Shape aMirrorShape = aBRepTrsf.Shape();

	TopoDS_Wire aMirroredWire = TopoDS::Wire(aMirrorShape);

	BRepBuilderAPI_MakeWire mkWire;
	mkWire.Add(aWire);
	mkWire.Add(aMirroredWire);
	TopoDS_Wire myWireProfile = mkWire.Wire();

	TopoDS_Face myFaceProfile = BRepBuilderAPI_MakeFace(myWireProfile);

	gp_Vec aPrismVec(0, 0, h);

	TopoDS_Shape myBody = BRepPrimAPI_MakePrism(myFaceProfile, aPrismVec);

	BRepFilletAPI_MakeFillet mkFillet(myBody);

	for (TopExp_Explorer anExpSF(myBody, TopAbs_EDGE); anExpSF.More(); anExpSF.Next())
	{
		TopoDS_Edge anEdge = TopoDS::Edge(anExpSF.Current());
		mkFillet.Add(t / 12.0, anEdge);
	}

	myBody = mkFillet.Shape();

	gp_Pnt neckLocation(0, 0, h);
	gp_Dir neckAxis = gp::DZ();
	gp_Ax2 neckAx2(neckLocation, neckAxis);

	Standard_Real myNeckRadius = t / 4.;
	Standard_Real myNeckHeight = h / 10.;
	BRepPrimAPI_MakeCylinder MKCylinder(neckAx2, myNeckRadius, myNeckHeight);
	TopoDS_Shape myNeck = MKCylinder.Shape();

	myBody = BRepAlgoAPI_Fuse(myBody, myNeck);

	SetShape(myBody, false);

#endif
}

//===================================================================
GOCCBox::GOCCBox() : GOCCObject(GOCC_BOX)
{
}

bool GOCCBox::Update(bool b)
{
#ifdef HAS_OCC
	// make the OCC object
	MakeBox();

	// build the GObject structure
	BuildGObject();

	// build the viz mesh
	BuildGMesh();

	return true;
#else
	return false;
#endif // HAS_OCC
}

void GOCCBox::MakeBox()
{
#ifdef HAS_OCC
	TopoDS_Solid box1 = BRepPrimAPI_MakeBox(gp_Pnt(0, 0, 0), 1, 1, 1);
	SetShape(box1, false);
#endif
}