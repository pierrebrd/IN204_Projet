#ifndef sceneHPP
#define sceneHPP

#include "objects.hpp"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <tuple>
#include <algorithm>


std::optional<std::pair<Point_3D,Object*>> findNextIntersection(Ray myRay,std::vector<Object*> *objects){
    // A partir d'un rayon donné (qui arrive à la caméra), lui associer un RGB en fonction des obstacles que rencontre le rayon.
    
    // Rayon d'ombre
    // On détermine l'intersection la plus proche de la caméra avec un objet
    std::vector<Object*>::iterator it = objects->begin();
    std::optional<Point_3D> closestIntersec = (*it)->find_intersection(myRay);
    Object* closestObjectPt = *it; // TODO : Vérifier que c'est un itérateur valide genre qu'on n'est pas à la fin du vecteur
    while (!closestIntersec.has_value() && it!=objects->end()){
        // On cherche la première intersection non vide
        closestIntersec = (*it)->find_intersection(myRay);
        closestObjectPt = *it;
        ++it;
    }

    // Le rayon n'intersecte avec aucun objet de la scène
    if (!closestIntersec.has_value()){
        return std::nullopt;
    }

    // Le rayon intersecte avec au moins un objet
    Point_3D rayOrigin = myRay.get_origin();
    for (; it!=objects->end(); it++){
        std::optional<Point_3D> currentIntersec = (*it)->find_intersection(myRay);
        if (currentIntersec.has_value()){
            if (rayOrigin.distanceTo(currentIntersec.value()) < rayOrigin.distanceTo(closestIntersec.value())){
                closestIntersec = currentIntersec;
                closestObjectPt = *it;
            }
        }
    }
    return std::make_pair(closestIntersec.value(),closestObjectPt);
}

float determineLightIntensity(Point_3D intersection, Object* intersected_object,std::vector<Light> *lights, std::vector<Object*> *objects){
    std::vector<Light>::iterator it = lights->begin();
    
    Vector_3D normal = intersected_object->get_normal(intersection);

    float totalIntensity = 0;
    // Rayons d'ombre
    for (;it!=lights->end(); it++){
        Ray nextRay(intersection,Vector_3D(intersection,it->get_position()));
        auto nextIntersection = findNextIntersection(nextRay,objects);
        float currentIntensity;
        if (nextIntersection.has_value() && nextIntersection.value().first.distanceTo(intersection) > 0.001){ // arbitraire de fou
            currentIntensity = 0;
        }
        else{
            currentIntensity = it->compute_PointLight(intersection,normal);
            totalIntensity = totalIntensity + currentIntensity;
        }
    }

    // Rayon réfléchi
    // On permet aux rayons de se réflechir une fois. 
    // A reflection ray is traced in the mirror-reflection direction. The closest object it intersects is what will be seen in the reflection
    

    return std::min(totalIntensity,1.0f);
}

float determineLightIntensity_ReflectedRay(Point_3D intersection, Object* intersected_object,std::vector<Light> *lights, std::vector<Object*> *objects, Ray incidentRay){
    Vector_3D normal = intersected_object->get_normal(intersection);
    Vector_3D reflectedRayDirection = intersected_object->determineReflectedRay(incidentRay,intersection,normal);
    Point_3D nextIntersection = 
}

std::tuple<uint8_t,uint8_t,uint8_t> compute_color(Ray myRay,std::vector<Object*> *objects, std::vector<Light> *lights){
    auto nextIntersect = findNextIntersection(myRay,objects);
    if (!nextIntersect.has_value()){
        return {0,0,0};
    }
    else {
        Point_3D intersection = nextIntersect.value().first;
        Object* intersected_object = nextIntersect.value().second;
        float lightIntensity = determineLightIntensity(intersection,intersected_object,lights,objects);
        auto object_color = intersected_object->get_color();
        return {static_cast<uint8_t>(lightIntensity*std::get<0>(object_color)),
                static_cast<uint8_t>(lightIntensity*std::get<1>(object_color)),
                static_cast<uint8_t>(lightIntensity*std::get<2>(object_color))};
    }
    
}


void image_creator(Camera cam, std::vector<Object*> *objects, std::vector<Light> *lights, std::string filename){
    // This function creates an image of the scene and saves it as a ppm file

    // We open the ppm file as a stream
    std::ofstream file(filename); // We create the file

    if (!file.is_open()) {
        std::cerr << "Impossible to open the file " << filename << std::endl;
        return;
    }

    file << "P3" << std::endl;
    file << cam.get_resolution_width() << " " << cam.get_resolution_height() << std::endl;
    file << "255" << std::endl;
    for(unsigned i = 0 ; i < cam.get_resolution_height() ; i++){
        for (unsigned j = 0; j < cam.get_resolution_width(); j++){
            // We lauch the ray from the camera
            Ray initial_Ray = cam.ray_launcher(cam.get_resolution_height()-i-1,j);
            // We calculate the color of the pixel 
            std::tuple<uint8_t,uint8_t,uint8_t> colors = compute_color(initial_Ray,objects,lights);
            // We write the color in the file 
            file << static_cast<int>(std::get<0>(colors)) << " " << static_cast<int>(std::get<1>(colors)) << " " << static_cast<int>(std::get<2>(colors)) << " ";
            file.flush();
        }
        file << std::endl;
        std::cout << "line" << i << " done" << std::endl;
    }
    file.close();
    std::cout << "The image has been created and saved as " << filename << std::endl;
    return;
}





#endif