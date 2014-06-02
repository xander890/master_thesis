#ifndef DEFAULTMATERIALS_H
#define DEFAULTMATERIALS_H

#include <Mesh/scatteringmaterial.h>

enum DefaultScatteringMaterial{S_Marble, S_Chocolate_Milk, S_Soy_Milk_regular, S_Potato};

Mesh::ScatteringMaterial* getDefaultMaterial(DefaultScatteringMaterial material);


#endif // DEFAULTMATERIALS_H
