/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGDALVectorReader.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkGDALVectorReader
 * @brief   Read vector file formats using GDAL.
 *
 * vtkGDALVectorReader is a source object that reads vector files and uses
 * GDAL as the underlying library for the task. GDAL is required for this
 * reader. The output of the reader is a vtkMultiBlockDataSet
 *
 * This filter uses the ActiveLayer member to only load entries from the
 * specified layer (when ActiveLayer >= 0).
 *
 * @sa
 * vtkMultiBlockDataSet
*/

#ifndef vtkGDALVectorReader_h
#define vtkGDALVectorReader_h

#include "vtkMultiBlockDataSetAlgorithm.h"
#include "vtkIOGDALModule.h" // For export macro

#include <string> // for ivars
#include <map> // STL required.

class VTKIOGDAL_EXPORT vtkGDALVectorReader : public vtkMultiBlockDataSetAlgorithm
{
public:
  static vtkGDALVectorReader* New();
  void PrintSelf( ostream& os, vtkIndent indent ) override;
  vtkTypeMacro(vtkGDALVectorReader,vtkMultiBlockDataSetAlgorithm);

  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);

  /**
   * Return number of layers.
   */
  int GetNumberOfLayers();

  /**
   * Given a index return layer type (eg point, line, polygon).
   */
  int GetLayerType(int layerIndex=0);

  /**
   * Given a layer index return number of features (shapes).
   */
  int GetFeatureCount(int layerIndex=0);

  /**
   * Return the active layer type (eg point, line, polygon).
   */
  int GetActiveLayerType();

  /**
   * Return the number of features in the active layer (shapes).
   */
  int GetActiveLayerFeatureCount();

  //@{
  /**
   * Set and Get the active layer.
   * If ActiveLayer is less than 0 (the default is -1), then all
   * layers are read. Otherwise, only the specified layer is read.
   */
  vtkSetMacro(ActiveLayer,int);
  vtkGetMacro(ActiveLayer,int);
  //@}

  //@{
  /**
   * Set and Get whether features are appended to a single
   * vtkPolyData. Turning the option on is useful when a shapefile has
   * a number of features which could otherwise lead to a huge
   * multiblock structure.
   */
  vtkSetMacro(AppendFeatures, int);
  vtkGetMacro(AppendFeatures, int);
  vtkBooleanMacro(AppendFeatures, int);
  //@}

  /**
   * Return projection string belonging to each layer in WKT format.
   */
  std::map<int, std::string> GetLayersProjection();

  /**
   * Return projection string belonging to a layer in WKT format.
   */
  const char* GetLayerProjection(int layerIndex);

  /**
   * Return projection string belonging to a layer in PROJ.4 format
   *
   * \note The returned string has to be deleted (via delete[]) by the
   * calling program.
   */
  const char* GetLayerProjectionAsProj4(int layerIndex);

  //@{
  /**
   * Set/get whether feature IDs should be generated.
   * Some GDAL primitives (e.g., a polygon with a hole
   * in its interior) are represented by multiple VTK
   * cells. If you wish to identify the primitive
   * responsible for a VTK cell, turn this on. It is
   * off by default for backwards compatibility.
   * The array of feature IDs will be the active
   * cell-data pedigree IDs.
   */
  vtkSetMacro(AddFeatureIds,int);
  vtkGetMacro(AddFeatureIds,int);
  vtkBooleanMacro(AddFeatureIds,int);
  //@}

protected:
  vtkGDALVectorReader();
  ~vtkGDALVectorReader() override;

  int RequestInformation( vtkInformation*, vtkInformationVector**, vtkInformationVector* ) override;
  int RequestData( vtkInformation*, vtkInformationVector**, vtkInformationVector* ) override;

  int InitializeInternal();

  /// The name of the file that will be opened on the next call to RequestData()
  char* FileName;

  int ActiveLayer;
  int AppendFeatures;
  int AddFeatureIds;

  class Internal;

  /// Private per-file metadata
  vtkGDALVectorReader::Internal* Implementation;

  /// Global variable indicating whether the OGR library has been registered yet or not.
  static int OGRRegistered;

  /// Mapping of layer to projection.
  std::map<int, std::string> LayersProjection;

private:
  vtkGDALVectorReader(const vtkGDALVectorReader&) = delete;
  void operator=(const vtkGDALVectorReader&) = delete;
};

#endif // vtkGDALVectorReader_h
