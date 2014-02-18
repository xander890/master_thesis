#include "ResourceLoader.h"

#include <fstream>
#include <iostream>

#include <QApplication>
#include <QFile>
#include <QString>
#include <QDebug>
#include <QStringList>
#include <QByteArray>


using namespace std;

namespace GLGraphics
{

/// Return empty string if no resource parameter is found
std::string ResourceLoader::get_optional_resource_path(){
    QStringList list = QCoreApplication::arguments();
    if (list.size() >= 2){
        string path = list[1].toStdString();
        if (path[path.length()-1] != '/'){
            path = path + "/";
        }
        return path;
    }
    return "";
}

std::string ResourceLoader::compute_resource_path(const std::string &resourceName){
    string path = get_optional_resource_path();
    string qtResource;
    if (path.length()>0){
        qtResource = path + resourceName;
    }
    else {
        string res = resourceName;
        while (res[0]=='.'){
            res = res.substr(1);
            qtResource = ":"+res;
        }
    }
    return qtResource;
}

std::string ResourceLoader::load_text_resource(const std::string &resourceName){
    string qtResource = compute_resource_path(resourceName);
    QFile file( qtResource.c_str());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qWarning() << "Cannot open file " << qtResource.c_str();
        exit( EXIT_FAILURE );
    }
    QByteArray data = file.readAll();
    return string(data.data());

}

}
