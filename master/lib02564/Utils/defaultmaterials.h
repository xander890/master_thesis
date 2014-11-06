#ifndef DEFAULTMATERIALS_H
#define DEFAULTMATERIALS_H

#include <Mesh/scatteringmaterial.h>

enum DefaultScatteringMaterial{
    S_Chicken,
    S_Skin,
    S_Wholemilk,
    S_Whitegrapefruit,
    S_Beer,
    S_Soymilk,
    S_Coffee,
    S_Marble,
    S_Potato,
    S_Ketchup,
    S_Apple,
    S_Chocolate_Milk
};

Mesh::ScatteringMaterial* getDefaultMaterial(DefaultScatteringMaterial material);
Mesh::ScatteringMaterial* getDefaultMaterial(DefaultScatteringMaterial material, float scale);


#endif // DEFAULTMATERIALS_H
