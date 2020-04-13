#include "stdafx.h"
#include "FEModelConstraint.h"

FEModelConstraint::FEModelConstraint(int ntype, FEModel* fem, int nstep) : FEModelComponent(ntype, fem, nstep)
{

}

FESurfaceConstraint::FESurfaceConstraint(int ntype, FEModel* fem, int nstep) : FEModelConstraint(ntype, fem, nstep)
{

}

//=============================================================================
// FEVolumeConstraint
//-----------------------------------------------------------------------------

FEVolumeConstraint::FEVolumeConstraint(FEModel* ps, int nstep) : FESurfaceConstraint(FE_VOLUME_CONSTRAINT, ps, nstep)
{
	SetTypeString("Volume Constraint");

	AddBoolParam(false, "laugon", "augmented lagrangian");
	AddDoubleParam(0.2, "augtol", "augmentation tolerance");
	AddDoubleParam(1, "penalty", "penalty factor");
}

//=============================================================================
// FEWarpingConstraint
//-----------------------------------------------------------------------------

FEWarpingConstraint::FEWarpingConstraint(FEModel* fem) : FEModelConstraint(FE_WARP_CONSTRAINT, fem)
{
	SetTypeString("warp-image");

	AddChoiceParam(0, "laugon", "Lagrange method")->SetEnumNames("penalty\0augmented_lagrangian\0");
	AddDoubleParam(0.1, "altol", "Augmentation tolerance");
	AddDoubleParam(0, "penalty", "Penalty");
	AddDoubleParam(0, "blur", "blur");
	AddVecParam(vec3d(0, 0, 0), "range_min", "Min range");
	AddVecParam(vec3d(1, 1, 1), "range_max", "Max range");
}

//=============================================================================
// FENormalFlowSurface
//-----------------------------------------------------------------------------

FENormalFlowSurface::FENormalFlowSurface(FEModel* ps, int nstep) : FESurfaceConstraint(FE_NORMAL_FLUID_FLOW, ps, nstep)
{
	SetTypeString("Normal flow constraint");

	AddBoolParam(false, "laugon", "augmented lagrangian");
	AddDoubleParam(0.2, "tol", "augmentation tolerance");
	AddDoubleParam(1  , "penalty", "penalty factor");
	AddDoubleParam(0  , "minaug", "min. augmentations");
	AddDoubleParam(10 , "maxaug", "max. augmentations");
	AddDoubleParam(0  , "rhs", "rhs");
}

//=============================================================================
// FESymmetryPlane
//-----------------------------------------------------------------------------

FESymmetryPlane::FESymmetryPlane(FEModel* ps, int nstep) : FESurfaceConstraint(FE_SYMMETRY_PLANE, ps, nstep)
{
	SetTypeString("symmetry plane");

	AddBoolParam(true, "laugon", "augmented lagrangian");
	AddDoubleParam(0.2, "tol", "augmentation tolerance");
	AddDoubleParam(1  , "penalty", "penalty factor");
	AddDoubleParam(0  , "minaug", "min. augmentations");
	AddDoubleParam(10 , "maxaug", "max. augmentations");
}

//=============================================================================
// FEFrictionlessFluidWall
//-----------------------------------------------------------------------------

FEFrictionlessFluidWall::FEFrictionlessFluidWall(FEModel* ps, int nstep) : FESurfaceConstraint(FE_FRICTIONLESS_FLUID_WALL, ps, nstep)
{
    SetTypeString("frictionless fluid wall");

    AddBoolParam(false, "laugon", "augmented lagrangian");
    AddDoubleParam(0.2, "tol", "augmentation tolerance");
    AddDoubleParam(1  , "penalty", "penalty factor");
    AddDoubleParam(0  , "minaug", "min. augmentations");
    AddDoubleParam(10 , "maxaug", "max. augmentations");
}
