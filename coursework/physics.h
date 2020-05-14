#define WIDTH 640
#define HEIGHT 480

using namespace std;
using namespace glm;

// FUNCTION DECLARATIONS
vector<ModelTriangle> gravity(vector<ModelTriangle> triangles,vector<float> velocities);
void initialiseVelocities(vector<Colour> materials);
bool collision(vector<ModelTriangle> triangles, Colour material);
float bounce(int);
void updateVelocities(vector<ModelTriangle> triangles, vector<Colour> materials);
vector<ModelTriangle> liftCubes(vector<ModelTriangle> triangles);
void initialiseCORs(vector<Colour> materials);
vector<ModelTriangle> moveCubesAboveFloor(vector<ModelTriangle> triangles, vector<Colour> materials);
vector<vector<ModelTriangle>> updatePlanetPositions(vector<vector<ModelTriangle>> planets);
vec3 rotatePoint(vec3 point, float t);
vector<ModelTriangle> updatePlanets(vector<vector<ModelTriangle>> planetsVector);

// GLOBAL VARIABLES
vector<float> velocities;
vector<float> CORs;
float timeSpeed = 0.1;


// GRAVITY FUNCTIONS ///////////////////////////////////////////////////

// APPLY GRAVITY 
vector<ModelTriangle> gravity(vector<ModelTriangle> triangles,vector<float> velocities, vector<Colour> materials) {
    int numOfTri = triangles.size();
    vector<ModelTriangle> newTriangles;
    for (int i = 0; i < numOfTri; i++){
        ModelTriangle triangle = triangles[i];
        if(triangle.fall){
            int index = 0;
            int numOfMaterials = materials.size();
            for (int k = 0; k < numOfMaterials; k++){
                if (triangle.colour.name == materials[k].name){
                    index = k;
                }
            }
            float velocity = velocities[index];
            float displacement = (velocity*timeSpeed);
            for (int j = 0; j < 3; j++){
                triangle.vertices[j].y = triangle.vertices[j].y + displacement;
            } 
        }
        newTriangles.push_back(triangle);
    }
    newTriangles = moveCubesAboveFloor(newTriangles,materials);
    updateVelocities(newTriangles,materials);
    return newTriangles;
}

// SETTING INITIAL VELOCITIES OF GRAVITY AFFECTED MATERIALS
void initialiseVelocities(vector<Colour> materials) {
    vector<float> initialVelocities;
    int numOfMaterials = materials.size();
    for (int i = 0; i < numOfMaterials; i++){
        Colour material = materials[i];
        float velocity = 1;
        if((material.name != "Mirror") && (material.name != "Glass")){
            velocity = std::numeric_limits<float>::infinity();
        }
        initialVelocities.push_back(velocity);
    }
    velocities = initialVelocities;
}

// SET TO COLLIDE WITH FLOOR OF CORNELL BOX
bool collision(vector<ModelTriangle> triangles, Colour material){
    int numOfTri = triangles.size();
    for (int i = 0; i < numOfTri; i++){
        if (triangles[i].colour.name == material.name){
            for (int j = 0; j < 3; j++){
                if (triangles[i].vertices[j].y <= -0.162686){
                    return true;
                }
            }    
        }
    }
    return false;
}

// BOUNCE REDUCES VELOCITY
float bounce(int index){
    float newVelocity = - (velocities[index] * CORs[index]);
    if (newVelocity > 0.7) {
       return newVelocity; 
    } else {
        return 0;
    }
    
}

// INCREASING ACCELERATION WHEN FALLING, G =9 .8
void updateVelocities(vector<ModelTriangle> triangles, vector<Colour> materials){
    int numOfMaterials = materials.size();
    for (int i = 0; i < numOfMaterials; i++){
        if((materials[i].name == "Mirror") || (materials[i].name == "Glass")){
            float newVelocity;
            if (collision(triangles, materials[i])){
                    newVelocity = bounce(i);
            } else {
                newVelocity = velocities[i] - (9.8 * timeSpeed);
            }
            velocities[i] = newVelocity; 
        }
    }
}

// INITIAL LIFT OF CUBES
vector<ModelTriangle> liftCubes(vector<ModelTriangle> triangles){
    int numOfTri = triangles.size();
    for (int i = 0; i < numOfTri; i++){
        if(triangles[i].colour.name == "Mirror"){
            for (int j = 0; j < 3; j++){
                triangles[i].vertices[j].y = triangles[i].vertices[j].y + 4;
            }
        }else if(triangles[i].colour.name == "Glass"){
            for (int j = 0; j < 3; j++){
                triangles[i].vertices[j].y = triangles[i].vertices[j].y + 6;
            }
        }
    }
    return triangles;
}


void initialiseCORs(vector<Colour> materials) {
    vector<float> initialCORs;
    int numOfMaterials = materials.size();
    for (int i = 0; i < numOfMaterials; i++){
        Colour material = materials[i];
        float COR = 0.5;
        if((material.name != "Mirror") && (material.name != "Glass")){
            COR = std::numeric_limits<float>::infinity();
        }
        initialCORs.push_back(COR);
    }
    CORs = initialCORs;
}


vector<ModelTriangle> moveCubesAboveFloor(vector<ModelTriangle> triangles, vector<Colour> materials) {
    int numOfTri = triangles.size();
    int numOfMaterials = materials.size();
    for (int j = 0; j < numOfMaterials; j++){
        float distanceBelowFloor = 0;
        if ((materials[j].name == "Mirror") || (materials[j].name == "Glass")){
            for (int i = 0; i < numOfTri; i++){
                if (triangles[i].colour.name == materials[j].name){
                    for (int k = 0; k < 3; k++){
                        if (triangles[i].vertices[k].y < -0.162686){
                            if (triangles[i].vertices[k].y < distanceBelowFloor){
                                distanceBelowFloor = triangles[i].vertices[k].y + 0.162686;
                            }
                        }
                    }    
                }
            }

            if (distanceBelowFloor < 0 ){
                for (int i = 0; i < numOfTri; i++){
                    if (triangles[i].colour.name == materials[j].name){
                        for (int k = 0; k < 3; k++){
                            triangles[i].vertices[k].y = triangles[i].vertices[k].y - distanceBelowFloor;
                        }
                    }
                }
            }
        }
    }
    return triangles;
}


// PLANET PHYSICS //////////////////////////////////////////////////////

// UPDATES PLANET POSITION BASED ON ORBITAL VELOCITIES 
vector<vector<ModelTriangle>> updatePlanetPositions(vector<vector<ModelTriangle>> planets){
  // NOT REAL
  vector<float> orbitSpeeds{-1,9,8,7,6,4.5,4,3,2}; 
  vector<vector<ModelTriangle>> newPlanets;
  int n = planets.size();
  for(int i = 0; i < n; i++){
    vector<ModelTriangle> planet = planets[i];
    int ntri = planet.size();
    for(int j = 0; j < ntri; j++){
        for(int k = 0; k < 3; k++){
            planet[j].vertices[k] = rotatePoint(planet[j].vertices[k], orbitSpeeds[i]);
            planet[j].normals[k]  = rotatePoint(planet[j].normals[k], orbitSpeeds[i]);
        }
    }
    newPlanets.push_back(planet);
  }
  return newPlanets;
}

// ROTATES POINT ABOUT (0,0,0)
vec3 rotatePoint(vec3 point, float velocity){
    float theta = 0.5*velocity;
    mat3 rotationMatrix = mat3(cos ( theta * PI / 180.0 ),0, sin ( theta * PI / 180.0 ),0,1,0, -(sin ( theta * PI / 180.0 )),0, cos ( theta * PI / 180.0 ));
    vec3 newPoint = point * (rotationMatrix);
    return newPoint;
}

// APPEND PLANETS INTO ONE VECTOR
vector<ModelTriangle> updatePlanets(vector<vector<ModelTriangle>> planetsVector){
    vector<ModelTriangle> newPlanets;
    int n = planetsVector.size();
    for (int i = 0; i < n; i++){
        newPlanets.insert(newPlanets.end(), planetsVector[i].begin(), planetsVector[i].end());
    }
    return newPlanets;
}

