//============================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//
//  Copyright 2015 Sandia Corporation.
//  Copyright 2015 UT-Battelle, LLC.
//  Copyright 2015 Los Alamos National Security.
//
//  Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
//  the U.S. Government retains certain rights in this software.
//
//  Under the terms of Contract DE-AC52-06NA25396 with Los Alamos National
//  Laboratory (LANL), the U.S. Government retains certain rights in
//  this software.
//============================================================================

#include "vtkmConnectivityExec.h"

namespace vtkm {
namespace exec {

//------------------------------------------------------------------------------
template <typename Device> ConnectivityVTKAOS<Device>::ConnectivityVTKAOS()
{
}

//------------------------------------------------------------------------------
template <typename Device>
ConnectivityVTKAOS<Device>::ConnectivityVTKAOS(
    const ShapePortalType& shapePortal,
    const ConnectivityPortalType& connPortal,
    const IndexOffsetPortalType& indexOffsetPortal)
  : Shapes(shapePortal),
    Connectivity(connPortal),
    IndexOffsets(indexOffsetPortal)
{
}

//------------------------------------------------------------------------------
template <typename Device>
vtkm::Id ConnectivityVTKAOS<Device>::GetNumberOfElements() const
{
  return this->Shapes.GetNumberOfValues();
}

//------------------------------------------------------------------------------
template <typename Device>
typename ConnectivityVTKAOS<Device>::CellShapeTag
ConnectivityVTKAOS<Device>::GetCellShape(vtkm::Id index) const
{
  // VTK-m and VTK cell shape numeric values are identical so no
  // conversion is needed
  return CellShapeTag(this->Shapes.Get(index));
}

//------------------------------------------------------------------------------
template <typename Device>
typename ConnectivityVTKAOS<Device>::IndicesType
ConnectivityVTKAOS<Device>::GetIndices(vtkm::Id index) const
{
  vtkm::Id offset = this->IndexOffsets.Get(index);
  auto length = static_cast<vtkm::IdComponent>(this->Connectivity.Get(offset));
  return IndicesType(this->Connectivity, length, offset + 1);
}

//------------------------------------------------------------------------------
template <typename Device>
ConnectivityVTKSingleType<Device>::ConnectivityVTKSingleType()
  : Connectivity(), NumberOfCells(0), NumberOfPointsPerCell(0), ShapeType(0)
{
}

//------------------------------------------------------------------------------
template <typename Device>
ConnectivityVTKSingleType<Device>::ConnectivityVTKSingleType(
    const ConnectivityPortalType& connPortal, vtkm::Id numCells,
    vtkm::IdComponent numPointsPerCell, vtkm::UInt8 shapeType)
  : Connectivity(connPortal),
    NumberOfCells(numCells),
    NumberOfPointsPerCell(numPointsPerCell),
    ShapeType(shapeType)
{
}

//------------------------------------------------------------------------------
template <typename Device>
vtkm::Id ConnectivityVTKSingleType<Device>::GetNumberOfElements() const
{
  return this->NumberOfCells;
}

//------------------------------------------------------------------------------
template <typename Device>
typename ConnectivityVTKSingleType<Device>::CellShapeTag
ConnectivityVTKSingleType<Device>::GetCellShape(
    vtkm::Id vtkmNotUsed(index)) const
{
  // VTK-m and VTK cell shape numeric values are identical so no
  // conversion is needed
  return this->ShapeType;
}

//------------------------------------------------------------------------------
template <typename Device>
typename ConnectivityVTKSingleType<Device>::IndicesType
ConnectivityVTKSingleType<Device>::GetIndices(vtkm::Id index) const
{
  // compute the offset, accounting for the VTK padding per cell
  const vtkm::Id offset = index * (this->NumberOfPointsPerCell + 1);
  // we do offset + 1 to skip the padding on the current cell
  return IndicesType(this->Connectivity, this->NumberOfPointsPerCell,
                     offset + 1);
}




//------------------------------------------------------------------------------
template <typename Device> ReverseConnectivityVTK<Device>::ReverseConnectivityVTK()
{
}

//------------------------------------------------------------------------------
template <typename Device>
ReverseConnectivityVTK<Device>::ReverseConnectivityVTK(
    const ConnectivityPortalType& connPortal,
    const IndexOffsetPortalType& indexOffsetPortal)
  : Connectivity(connPortal),
    IndexOffsets(indexOffsetPortal)
{
}

//------------------------------------------------------------------------------
template <typename Device>
vtkm::Id ReverseConnectivityVTK<Device>::GetNumberOfElements() const
{
  return this->IndexOffsets.GetNumberOfValues() - 1;
}

//------------------------------------------------------------------------------
template <typename Device>
typename ReverseConnectivityVTK<Device>::IndicesType
ReverseConnectivityVTK<Device>::GetIndices(vtkm::Id index) const
{
  const vtkm::Id offset = this->IndexOffsets.Get(index);
  const vtkm::Id next = this->IndexOffsets.Get(index+1);
  vtkm::IdComponent length = static_cast<vtkm::IdComponent>(next - offset);
  return IndicesType(this->Connectivity, length, offset);
}

// template methods we want to compile only once
template class ConnectivityVTKAOS<vtkm::cont::DeviceAdapterTagSerial>;
template class ConnectivityVTKSingleType<vtkm::cont::DeviceAdapterTagSerial>;
template class ReverseConnectivityVTK<vtkm::cont::DeviceAdapterTagSerial>;

#ifdef VTKM_ENABLE_TBB
template class ConnectivityVTKAOS<vtkm::cont::DeviceAdapterTagTBB>;
template class ConnectivityVTKSingleType<vtkm::cont::DeviceAdapterTagTBB>;
template class ReverseConnectivityVTK<vtkm::cont::DeviceAdapterTagTBB>;
#endif

#ifdef VTKM_ENABLE_OPENMP
template class ConnectivityVTKAOS<vtkm::cont::DeviceAdapterTagOpenMP>;
template class ConnectivityVTKSingleType<vtkm::cont::DeviceAdapterTagOpenMP>;
template class ReverseConnectivityVTK<vtkm::cont::DeviceAdapterTagOpenMP>;
#endif

#ifdef VTKM_ENABLE_CUDA
template class ConnectivityVTKAOS<vtkm::cont::DeviceAdapterTagCuda>;
template class ConnectivityVTKSingleType<vtkm::cont::DeviceAdapterTagCuda>;
template class ReverseConnectivityVTK<vtkm::cont::DeviceAdapterTagCuda>;
#endif
}
}
