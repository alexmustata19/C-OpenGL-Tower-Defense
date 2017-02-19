/*Mustata Alexandru-Ionut
 *Grupa: 331CB
 *Tema 2 EGC: Build and Survive
 */

#include "Tema2.h"

#include <vector>
#include <string>
#include <iostream>

#include <Core/Engine.h>

#include "Player.h"
#include "Bullet.h"
#include "Grenade.h"
#include "Enemy.h"
#include "Tower.h"

using namespace std;

bool miniMap;
glm::mat4 view;

Player steve;
vector<Bullet> bullets;
vector<Grenade> grenades;
vector<Enemy> enemies;
vector<Tower> towers;

float gateAngle = 0;
float nextGateAngle = 30;
float gateAnimationTimeStart = 0;
float gateAnimationDuration = 2;
float gateAnimation = false;

bool gameOver = false;
float endGameTime = 0;
int lastNumberOfEnemiesKilled = 0;
int numberOfEnemiesKilled = 0;
int numberOfShots = 0;
int chanceInterval = 10;
int chanceFactor = 100;

bool pistol = true;
bool shotgun = false;
bool grenadeLauncher = false;

float pistolPower = 4;
float shotgunPower = 2;
float grenadeLauncherPower = 10;

float pistolMaxDistance = 10;
float shotgunMaxDistance = 15;
float grenadeLauncherMaxDistance = 7.5;

float pistolCooldown = 1;
float shotgunCooldown = 2;
float grenadeLauncherCooldown = 5;

float lastPistolShot = 0;
float lastShotgunShot = 0;
float lastGrenadeLauncherShot = 0;

float pistolSpeed = 0.1;
float shotgunSpeed = 0.2;
float grenadeLauncherSpeed = 0.15;

float reper = 15;
float lastEnemyEntry = 0;
float lastEnemyLife = 1;
float enemyEntryInterval = 5;
float enemySpeed = 0.02;

void Tema2::SetViewportArea(const ViewportSpace & viewSpace, glm::vec3 colorColor, bool clear)
{
	glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

	glEnable(GL_SCISSOR_TEST);
	glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);

	GetSceneCamera()->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
	GetSceneCamera()->Update();
}

Tema2::Tema2()
{
}

Tema2::~Tema2()
{
}

void Tema2::Init()
{
	/*Setarea dimensiunilor celor doua suprafete de vizualizare pentru 
	minimap si pentru timpul de reincarcare al armelor.*/
	int width = window->GetResolution().x;
	int height = window->GetResolution().y;
	int minimum = width < height ? width : height;
	viewSpaceMiniMap.x = width - minimum / 3 - width / 100;
	viewSpaceMiniMap.y = height / 100;
	viewSpaceMiniMap.width = minimum / 3;
	viewSpaceMiniMap.height = minimum / 3;
	viewSpaceWeaponsStats.x = width / 100;
	viewSpaceWeaponsStats.y = height / 100;
	viewSpaceWeaponsStats.width = minimum / 3;
	viewSpaceWeaponsStats.height = minimum / 9;

	//Instantierea player-ului si a celor doua camere.
	steve = Player(glm::vec3(0, 0, 0.0f), 90 * TO_RADIANS);
	steve.height = 0.7;
	camera = new Laborator::Camera();
	camera->Set(glm::vec3(0, 0, 3.5f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	camera->RotateThirdPerson_OX(RADIANS(-25));
	cameraTP = camera;
	cameraFP = new Laborator::Camera();
	cameraFP->distanceToTarget = 0;
	cameraFP->Set(glm::vec3(camera->GetTargetPosition().x, steve.height, camera->GetTargetPosition().z), glm::vec3(0, steve.height, 0), glm::vec3(0, 1, 0));
	steve.position = glm::vec3(camera->GetTargetPosition());

	/*Plasarea celor 3 turnuri.*/
	Tower t = Tower(glm::vec3(5, 0, 0), 0);
	towers.push_back(t);
	t = Tower(glm::vec3(0, 0, 5), 0);
	towers.push_back(t);
	t = Tower(glm::vec3(-5, 0, 0), 0);
	towers.push_back(t);

	/*Incarcarea mesh-urilor necesare desenarii obiectelor din joc.*/
	{
		Mesh* mesh = new Mesh("box");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Tema2", "box.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Tema2", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("Steve");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Tema2", "Steve.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("tower");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Tema2", "tower.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("Toro");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Tema2", "toro.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("beginGate");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Tema2", "trash gate.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("endGate");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Tema2", "SecurityGate.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}
}

void Tema2::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
	
}

//Functia sterge gloantele care si-au depasit raza de actiune.
void Tema2::deleteOutOfRangeBullets() {
	int count = 0;
	while (count < bullets.size()) {
		float distance = glm::sqrt(glm::pow(bullets[count].position.x - bullets[count].startPosition.x, 2) + 
			glm::pow(bullets[count].position.z - bullets[count].startPosition.z, 2));
		if (distance >= bullets[count].maxDistance) {
			bullets.erase(bullets.begin() + count);
		}
		else {
			count++;
		}
	}
}

//Functia sterge grenadele care au explodat.
void Tema2::deleteUsedGrenades() {
	int count = 0;
	while (count < grenades.size()) {
		if (grenades[count].endExplosion) {
			grenades.erase(grenades.begin() + count);
		}
		else {
			count++;
		}
	}
}

/*Functia realizeaza generarea inamicilor. Cate un inamic intra
in joc la un anumit interval. Viata inamicilor creste de la un
inamic generat la altul si intervalul de generare este scazut
pentru a mari dificultatea jocului.*/
void Tema2::generateEnemies() {
	float thisMoment = Engine::GetElapsedTime();
	if (thisMoment - lastEnemyEntry >= enemyEntryInterval) {
		Enemy aux = Enemy(glm::vec3(reper, Enemy::height, -reper), lastEnemyLife, enemySpeed);
		enemies.push_back(aux);
		if (enemyEntryInterval > 3) {
			enemyEntryInterval -= 0.1;
		}
		cout << "Inamic cu viata " << lastEnemyLife << " a intrat in joc.\n";
		lastEnemyLife += 0.5;
		lastEnemyEntry = thisMoment;
	}
}

/*Functia realizeaza desenarea inamicilor si miscarea acestora
de-a lungul traiectoriei prevazute. De asemenea, functia se ocupa
cu desenarea inamicilor in timpul animatiei de cadere cat si de
desenarea acestora pe minimap.*/
void Tema2::drawEnemies() {
	float thisMoment = Engine::GetElapsedTime();
	for (int i = 0; i < enemies.size(); i++) {
		if (enemies[i].beginAnimation == 0) {
			//Desenarea pe traiectorie, inamicul inca traieste.
			glm::mat4 modelMatrix = glm::mat4(1);

			//Efectuarea unui pas pe traiectorie.
			modelMatrix = glm::translate(modelMatrix, enemies[i].position);
			if (enemies[i].segment1 && !miniMap) {
				modelMatrix = glm::rotate(modelMatrix, -90 * TO_RADIANS, glm::vec3(0, 1, 0));
				enemies[i].position.x -= enemies[i].speed;
			}
			if (enemies[i].segment2 && !miniMap) {
				modelMatrix = glm::rotate(modelMatrix, 45 * TO_RADIANS, glm::vec3(0, 1, 0));
				enemies[i].position.x += enemies[i].speed*0.707;
				enemies[i].position.z += enemies[i].speed*0.707;
			}
			if (enemies[i].segment3 && !miniMap) {
				modelMatrix = glm::rotate(modelMatrix, -90 * TO_RADIANS, glm::vec3(0, 1, 0));
				enemies[i].position.x -= enemies[i].speed;
			}
			if (enemies[i].position.x <= -reper && enemies[i].position.z == -reper && enemies[i].segment1 && !miniMap) {
				enemies[i].position.x = -reper;
				enemies[i].segment1 = false;
				enemies[i].segment2 = true;
				enemies[i].segment3 = false;
			}
			if (enemies[i].position.x >= reper && enemies[i].position.z >= reper && enemies[i].segment2 && !miniMap) {
				enemies[i].position.x = reper;
				enemies[i].position.z = reper;
				enemies[i].segment1 = false;
				enemies[i].segment2 = false;
				enemies[i].segment3 = true;
			}

			//Desenare in scena si pe minimap.
			modelMatrix = glm::rotate(modelMatrix, -90 * TO_RADIANS, glm::vec3(1, 0, 0));
			if (!miniMap) {
				modelMatrix = glm::scale(modelMatrix, glm::vec3(Enemy::scale));
				RenderMesh(meshes["Toro"], shaders["VertexNormal"], modelMatrix);
			}
			else {
				modelMatrix = glm::rotate(modelMatrix, 90 * TO_RADIANS, glm::vec3(0, 1, 0));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(1));
				RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
			}
		}
		else if(thisMoment - enemies[i].beginAnimation <= Enemy::animationTime){
			//Desenarea in timpul animatie de cadere si micsorare.
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, enemies[i].position);
			float fraction = 1.0 * (thisMoment - enemies[i].beginAnimation) / Enemy::animationTime;
			float angle = TO_RADIANS * 0 - (TO_RADIANS *(0 - 90)) * fraction;

			if (enemies[i].segment1) {
				modelMatrix = glm::rotate(modelMatrix, -90 * TO_RADIANS, glm::vec3(0, 1, 0));
				//Rotatia de cadere
				modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0, 0, 1));
			}
			if (enemies[i].segment2) {
				modelMatrix = glm::rotate(modelMatrix, 45 * TO_RADIANS, glm::vec3(0, 1, 0));
				//Rotatia de cadere
				modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(-1, 0, 1));
			}
			if (enemies[i].segment3) {
				modelMatrix = glm::rotate(modelMatrix, -90 * TO_RADIANS, glm::vec3(0, 1, 0));
				//Rotatia de cadere
				modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0, 0, 1));
			}

			//Micsorare si cadere
			float s = Enemy::scale - (Enemy::scale - Enemy::deadScale) * fraction;
			enemies[i].position.y = Enemy::height - Enemy::height*fraction;

			//Desenare in scena si pe minimap.
			modelMatrix = glm::rotate(modelMatrix, -90*TO_RADIANS, glm::vec3(1,0,0));
			if (!miniMap) {
				modelMatrix = glm::scale(modelMatrix, glm::vec3(s));
				RenderMesh(meshes["Toro"], shaders["VertexNormal"], modelMatrix);
			}
			else {
				modelMatrix = glm::rotate(modelMatrix, 90 * TO_RADIANS, glm::vec3(0, 1, 0));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(s * 2.5));
				RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
			}
		}
		else if (thisMoment - enemies[i].beginAnimation > Enemy::animationTime) {
			//Moartea efectiva a inamicului.
			enemies[i].endAnimation = true;
		}
	}
	deleteDeadEnemies();
}

/*Functia realizeaza animatia de cadere a personajului in momentul
in care moare. Acesta va pica in fata, ramanand culcat la pamant.
Cand moare acesta se transforma intr-un cub rosu pe minimap.*/
void Tema2::steveFallAnimation() {
	float thisMoment = Engine::GetElapsedTime();
	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, steve.position);
	modelMatrix = glm::rotate(modelMatrix, steve.rotation, glm::vec3(0, 1, 0));
	if (thisMoment - endGameTime <= Player::fallAnimationTime) {
		float fraction = 1.0 * (thisMoment - endGameTime) / Player::fallAnimationTime;
		float angle = TO_RADIANS * 0 - (TO_RADIANS *(0 - 90)) * fraction;
		if (!miniMap) {
			modelMatrix = glm::rotate(modelMatrix, -angle, glm::vec3(1, 0, 0));
		}
		else {
			modelMatrix = glm::rotate(modelMatrix, -angle, glm::vec3(0, 0, 1));
		}
	}
	else {
		if (!miniMap) {
			modelMatrix = glm::rotate(modelMatrix, -TO_RADIANS * 90, glm::vec3(1, 0, 0));
		}
		else {
			modelMatrix = glm::rotate(modelMatrix, -TO_RADIANS * 90, glm::vec3(0, 0, 1));
		}
		
	}
	if (!miniMap) {
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
		RenderMesh(meshes["Steve"], shaders["VertexNormal"], modelMatrix);
	}
	else {
		modelMatrix = glm::rotate(modelMatrix, TO_RADIANS * 90, glm::vec3(1, 0, 0));//albastru
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1));
		RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
	}
}

/*Functia calculeaza ciocnirile dintre proiectile si inamici.*/
void Tema2::computeCollisions() {
	float thisMoment = Engine::GetElapsedTime();

	//Calcularea coliziunilor gloante-oponenti.
	for (int i = 0; i < bullets.size(); i++) {
		for (int j = 0; j < enemies.size(); j++) {
			float distance = glm::sqrt(
				glm::pow(bullets[i].position.x - enemies[j].position.x, 2) + 
				glm::pow(bullets[i].position.z - enemies[j].position.z, 2));
			float r = 1;
			if (bullets[i].type == "pistol" || bullets[i].type == "tower" ) {
				r = 0.5f;
			}
			else if (bullets[i].type == "shotgun") {
				r = 0.75f;
			}
			if (distance <= r + Enemy::radius * Enemy::scale) {
				enemies[j].life -= bullets[i].power;

				/*Pentru a sterge gloantele este suficient sa fie translatate la o 
				distanta foarte mare de locul jocului, ca si cum au depasit raza de actiune.*/
				bullets[i].position = glm::vec3(1000);
				
				if (enemies[j].life <= 0 && enemies[j].beginAnimation == 0) {
					enemies[j].beginAnimation = thisMoment;
					numberOfEnemiesKilled++;
				}
			}
		}
	}

	//Stergerea gloantelor folosite.
	deleteOutOfRangeBullets();

	//Calcularea coliziunilor grenade-oponenti.
	for (int i = 0; i < grenades.size(); i++) {
		for (int j = 0; j < enemies.size(); j++) {
			float distance = glm::sqrt(
				glm::pow(grenades[i].position.x - enemies[j].position.x, 2) +
				glm::pow(grenades[i].position.z - enemies[j].position.z, 2));
			if (distance <= Grenade::radius * grenades[i].scale + Enemy::radius * Enemy::scale && grenades[i].beginExplosion == 0) {
				enemies[j].life -= grenades[i].power;
				grenades[i].beginExplosion = thisMoment;
				if (enemies[j].life <= 0 && enemies[j].beginAnimation == 0) {
					enemies[j].beginAnimation = thisMoment;
					numberOfEnemiesKilled++;
				}
			}
		}
	}
}

/*Functia realizeaza stergerea inamicilor care au ajuns la
poarta aparat de player sau care au fost distriusi pe drum.*/
void Tema2::deleteDeadEnemies() {
	int count = 0;
	float thisMoment = Engine::GetElapsedTime();
	while (count < enemies.size()) {
		if (enemies[count].endAnimation) {
			enemies.erase(enemies.begin() + count);
		}
		else if(enemies[count].position.x <= -reper && enemies[count].position.z == reper && enemies[count].segment3){
			enemies.erase(enemies.begin() + count);
			steve.decrementLifes();
			gateAnimation = true;
			gateAnimationTimeStart = thisMoment;
			if (steve.getLifes() == 0) {
				gameOver = true;
				endGameTime = thisMoment;
				
				//Afisare statistica joc.
				cout << "Jocul s-a sfarsit!\n";
				cout << "Jocul a durat " << endGameTime << " secunde.\n";
				cout << "Au fost trase " << numberOfShots << " proiectile.\n";
				if (numberOfEnemiesKilled == 0) {
					cout << "Nu a afost distrus niciun inamic.\n";
				}
				else if (numberOfEnemiesKilled == 1) {
					cout << "A fost distrus doar un inamic.\n";
				}
				else {
					cout << "Au fost distrusi " << numberOfEnemiesKilled << " inamici.\n";
				}
			}
		}
		else {
			count++;
		}
	}
}

/*Functia implementeaza apararea pe care turnurile o ofera player-ului.*/
void Tema2::towerDefense() {
	float thisMoment = Engine::GetElapsedTime();
	for (int i = 0; i < towers.size(); i++) {
		//Se determina cel mai apropiat inamic de turnul curent.
		Enemy closest;
		float minDist = 1000000;
		for (int j = 0; j < enemies.size(); j++) {
			float dist = glm::sqrt(
				glm::pow(towers[i].position.x - enemies[j].position.x, 2) + 
				glm::pow(towers[i].position.z - enemies[j].position.z, 2));
			if (minDist > dist) {
				closest = enemies[j];
				minDist = dist;
			}
		}
		//Se determina unghiul la care se afla inamicul fata de turn.
		glm::vec3 pos = glm::vec3(towers[i].position.x, Tower::height, towers[i].position.z);
		float radical = glm::sqrt(
			glm::pow(towers[i].position.z - closest.position.z, 2) +
			glm::pow(towers[i].position.x - closest.position.x, 2)
		);
		float cos = (towers[i].position.z - closest.position.z)/radical;
		float sin = (towers[i].position.x - closest.position.x) / radical;
		
		//Se actualizeaza orientarea turnului catre inamic.
		towers[i].rotationCos = cos;
		towers[i].rotationSin = sin;

		/*Daca timpul de reincarcare de la tragerea ultimului glont a trecut
		atunci se trage un nou glont spre cel mai apropiat inamic.*/
		if (thisMoment - towers[i].lastShot >= Tower::cooldown) {
			glm::vec3 direction = closest.position - pos;
			direction.y = 0;
			
			//Turnurile au sansa sa lanseze un mega bullet.
			int chance = rand() % chanceInterval;
			float power = Tower::power;
			if (chance == 0) {
				power *= chanceFactor;
				cout << "Turnul " << (i+1) << " a lansat un mega bullet.\n";
			}
			
			Bullet aux = Bullet(pos, glm::normalize(direction), 0, power, Tower::maxDistance, "tower", Tower::speed);
			aux.rotationCos = cos;
			aux.rotationSin = sin;
			bullets.push_back(aux);
			towers[i].lastShot = thisMoment;
			numberOfShots++;
		}
	}
}

/*Functia returneaza matricea de rotatie fata de axa OY cu un
unghi specificat prin cosinus si sinus.*/
glm::mat4 Tema2::myRotateOY(float cos, float sin) {
	return glm::transpose(
		glm::mat4(
			cos, 0, sin, 0,
			0, 1, 0, 0,
			-sin, 0, cos, 0,
			0, 0, 0, 1
		)
	);
}

/*Functia realizeaza desenarea turnurilor in scena principala cat si pe minimap.
Pe minimap turnurile sunt reprezentate de cuburi verzi.*/
void Tema2::drawTowers() {
	for (int i = 0; i < towers.size(); i++) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, towers[i].position);
		modelMatrix = modelMatrix * myRotateOY(towers[i].rotationCos, towers[i].rotationSin);
		if (!miniMap) {
			modelMatrix = glm::scale(modelMatrix, glm::vec3(Tower::scale));
			RenderMesh(meshes["tower"], shaders["VertexNormal"], modelMatrix);
		}
		else {
			modelMatrix = glm::rotate(modelMatrix, TO_RADIANS * 0, glm::vec3(1, 0, 0)); //verde
			modelMatrix = glm::scale(modelMatrix, glm::vec3(1));
			RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
		}
	}
}

/*Functia realizeaza desenarea player-ului in scena principala cat si pe minimap.
Pe minimap player-ul este desenat ca un cub albastru, in momentul in care va
muri va deveni rosu.*/
void Tema2::drawSteve() {
	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, steve.position);
	modelMatrix = glm::rotate(modelMatrix, steve.rotation, glm::vec3(0, 1, 0));
	if (!miniMap) {
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
		RenderMesh(meshes["Steve"], shaders["VertexNormal"], modelMatrix);
	}
	else {
		modelMatrix = glm::rotate(modelMatrix, TO_RADIANS * 90, glm::vec3(1, 0, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1));
		RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
	}
}

/*Functia realizeaza desenarea vietilor ca sfere in jurul player-ului.*/
void Tema2::drawLifes() {
	int lifes = steve.getLifes();
	glm::mat4 modelMatrix;
	
	if (lifes == 3) {
		float oxAngle = steve.getAngleOx();
		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, steve.position);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, steve.lifeRadius*glm::sin(oxAngle), steve.lifeRadius*glm::cos(oxAngle)));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, steve.lifeRadius / 2, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
		RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
	}

	if (lifes >= 2) {
		float oyAngle = steve.getAngleOy();
		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, steve.position);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(steve.lifeRadius*glm::cos(oyAngle), 0, steve.lifeRadius*glm::sin(oyAngle)));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, steve.lifeRadius / 2, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
		RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
	}

	if (lifes >= 1) {
		float ozAngle = steve.getAngleOz();
		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, steve.position);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(steve.lifeRadius*glm::cos(ozAngle), steve.lifeRadius*glm::sin(ozAngle), 0));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, steve.lifeRadius / 2, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
		RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
	}
}

/*Functia realizeaza desenarea gloantelor in timp ce se deplaseaza si
stergerea celor care au depasit raza maxima de actiune. Gloantele trase
de player si turnuri sunt cuburi, iar pusca trage cu paralelipipede.*/
void Tema2::drawBullets() {
	for (int i = 0; i < bullets.size(); i++) {
		bullets[i].position += bullets[i].direction * bullets[i].speed;
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, bullets[i].position);
		if (bullets[i].type == "pistol" || bullets[i].type == "shotgun") {
			modelMatrix = glm::rotate(modelMatrix, bullets[i].rotation, glm::vec3(0, 1, 0));
		}
		if (bullets[i].type == "tower") {
			modelMatrix = modelMatrix * myRotateOY(bullets[i].rotationCos, bullets[i].rotationSin);
		}
		if (bullets[i].type == "pistol" || bullets[i].type == "tower") {
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05f));
		}
		if (bullets[i].type == "shotgun"){
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f, 0.05f, 0.05f));
		}
		RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
	}
	deleteOutOfRangeBullets();
}

/*Functia realizeaza desenarea grenadelor in timp ce se deplaseaza si
explodeaza, dupa aceea se sterg grenadele explodate. Grenadele sunt sfere.*/
void Tema2::drawGrenades() {
	float thisMoment = Engine::GetElapsedTime();
	for (int i = 0; i < grenades.size(); i++) {
		if (grenades[i].beginExplosion == 0) {
			//Daca grenada nu a atins pamantul sau un inamic isi continua deplasarea.
			grenades[i].position += grenades[i].direction * grenades[i].speed;
			float distance = glm::sqrt(glm::pow(grenades[i].startPosition.x - grenades[i].position.x, 2) +
				glm::pow(grenades[i].startPosition.z - grenades[i].position.z, 2));
			grenades[i].position.y = steve.height + glm::sin(distance*0.5);
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, grenades[i].position);
			modelMatrix = glm::rotate(modelMatrix, grenades[i].rotation, glm::vec3(0, 1, 0));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(Grenade::originalScale));
			RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
			if (grenades[i].position.y <= 0) {
				grenades[i].beginExplosion = thisMoment;
			}
		}
		else {
			if (thisMoment - grenades[i].beginExplosion <= Grenade::explosionTime ) {
				//Daca grenada a atins un inamic sau pamantul va porni animatia de explozie.
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, grenades[i].position);
				modelMatrix = glm::rotate(modelMatrix, grenades[i].rotation, glm::vec3(0, 1, 0));
				float fraction =  Grenade::explosionMagnitude * (thisMoment - grenades[i].beginExplosion) * 1.0 / Grenade::explosionTime;
				grenades[i].scale = fraction * Grenade::originalScale;
				modelMatrix = glm::scale(modelMatrix, glm::vec3(grenades[i].scale));
				RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
			}
			else {
				//Daca timpul animatiei a fost depasit inseamna ca explozia s-a sfarsit.
				grenades[i].endExplosion = true;
			}
		}
	}

	//Stergerea grenadelor care au explodat.
	deleteUsedGrenades();
}

/*Functia realizeaza desenarea portii ce genereaza inamici.*/
void Tema2::drawBeginGate() {
	glm::mat4 modelMatrix = glm::mat4(1);
	if (!miniMap) {
		//Translatarea portii astfel incat inamicii sa fie generati din aceasta.
		modelMatrix = glm::translate(modelMatrix, glm::vec3(reper, 0, -reper + 1.5));
	}
	else {
		//Translatarea portii pe minimap.
		modelMatrix = glm::translate(modelMatrix, glm::vec3(reper, 0, -reper));
	}
	modelMatrix = glm::rotate(modelMatrix, 90 * TO_RADIANS, glm::vec3(0, 1, 0));
	if (!miniMap) {
		//Desenarea portii in scena principala.
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.005));
		RenderMesh(meshes["beginGate"], shaders["VertexNormal"], modelMatrix);
	}
	else {
		//Desenarea portii pe minimap ca paralelipiped.
		modelMatrix = glm::rotate(modelMatrix, 90 * TO_RADIANS, glm::vec3(0, 0, 1));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 3, 2));
		RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
	}
}

/*Functia realizeaza desenarea portii pe care player-ul trebuie sa o
apere. La fiecare viata pierduta de player, poarta se inclina spre
pamant cu inca 30 de grade, astfel incat cand jocul s-a sfarsit
poarta este prabusita.*/
void Tema2::drawEndGate() {
	float thisMoment = Engine::GetElapsedTime();
	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-reper, 0, reper));
	if (thisMoment - gateAnimationTimeStart <= gateAnimationDuration) {
		/*Daca jocul s-a sfarsit nu se mai tine cont de animatie, deoarece poarta
		trebuie sa ramana doborata la pamant, altfel se calculeaza unghiul intermediar.*/
		if (gateAngle == 90) {
			gateAnimation = false;
		}
		else {
			float fraction = 1.0 * (thisMoment - gateAnimationTimeStart) / gateAnimationDuration;
			float angle = TO_RADIANS * gateAngle - (TO_RADIANS *(gateAngle - nextGateAngle)) * fraction;
			modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0, 0, 1));
		}
	}
	else if(gateAnimation){
		/*Daca timpul animatiei s-a scurs, dar indicatorul a ramas setat
		atunci il resetez si actualizez unghiurile.*/
		gateAnimation = false;
		gateAngle = nextGateAngle;
		nextGateAngle += 30;
	}

	//Daca nu se deruleaza o animatie atunci poarta va avea un unghi fix.
	if (!gateAnimation) {
		modelMatrix = glm::rotate(modelMatrix, TO_RADIANS * gateAngle, glm::vec3(0, 0, 1));
	}

	if (!miniMap) {
		//Desenarea portii in scena principala.
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.01));
		RenderMesh(meshes["endGate"], shaders["VertexNormal"], modelMatrix);
	}
	else {
		//Desenarea portii pe minimap ca paralelipiped.
		modelMatrix = glm::rotate(modelMatrix, TO_RADIANS * 0, glm::vec3(0, 1, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 2, 3));
		RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
	}
}

/*Functia realizeaza apelurile necesare astfel incat scena sa fie
desenata corect, fie ca se deseneaza in modul principal, fie ca
se doreste desenarea pe minimap.*/
void Tema2::drawScene() {
	drawBeginGate();
	drawEndGate();
	if (!gameOver) {
		if (!miniMap) {
			generateEnemies();
			towerDefense();
			computeCollisions();
		}
		if (miniMap || (!shotgun)) {
			drawSteve();
		}
		if (!miniMap) {
			drawLifes();
		}
	}
	else {
		steveFallAnimation();
	}
	drawEnemies();
	drawTowers();
	if (!miniMap) {
		drawBullets();
		drawGrenades();
	}
}

/*Functia deseneaza sub forma unor bare care cresc timpul pana la
urmatoarea tragere disponibila. Cand bara este plina se poate trage
cu arma respectiva.*/
void Tema2::drawWeaponsStats() {
	glm::mat4 modelMatrix;
	float fraction;
	float thisMoment = Engine::GetElapsedTime();

	//Timp de incarcare al pistolului. Albastru
	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-9, 0, 6));
	modelMatrix = glm::rotate(modelMatrix, TO_RADIANS * 90, glm::vec3(1, 0, 0));
	if (thisMoment - lastPistolShot < pistolCooldown) {
		fraction = (thisMoment - lastPistolShot) / pistolCooldown;
	}
	else {
		fraction = 1;
	}
	modelMatrix = glm::scale(modelMatrix, glm::vec3(fraction * 36, 6, 3));
	RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
	
	//Timp de incarcare a pustii. Verde
	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-9, 0, 0));
	if (thisMoment - lastShotgunShot < shotgunCooldown) {
		fraction = (thisMoment - lastShotgunShot) / shotgunCooldown;
	}
	else {
		fraction = 1;
	}
	modelMatrix = glm::scale(modelMatrix, glm::vec3(fraction * 36, 3, 6));
	RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

	//Timp de incarcare al lansatorului de grenade. Rosu
	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-9, 0, -6));
	modelMatrix = glm::rotate(modelMatrix, TO_RADIANS * 90, glm::vec3(0, 0, 1));
	if (thisMoment - lastGrenadeLauncherShot < grenadeLauncherCooldown) {
		fraction = (thisMoment - lastGrenadeLauncherShot) / grenadeLauncherCooldown;
	}
	else {
		fraction = 1;
	}
	modelMatrix = glm::scale(modelMatrix, glm::vec3(3, fraction * 36, 6));
	RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
}

void Tema2::Update(float deltaTimeSeconds)
{	
	//Desenarea scenei principale.
	miniMap = false;
	projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
	view = camera->GetViewMatrix();
	DrawCoordinatSystem(view, projectionMatrix);
	drawScene();

	//Desenarea scenei pe minimap.
	miniMap = true;
	SetViewportArea(viewSpaceMiniMap, glm::vec3(0,0,0), false);
	view = glm::lookAt(glm::vec3(steve.position.x, 5, steve.position.z), glm::vec3(steve.position.x, 0, steve.position.z), glm::vec3(0, 0, -1));
	projectionMatrix = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, 0.01f, 10.0f);
	drawScene();
	DrawCoordinatSystem(view, projectionMatrix);

	//Desenarea timpilor pana la incarcarea armelor.
	SetViewportArea(viewSpaceWeaponsStats, glm::vec3(0, 0, 0), false);
	view = glm::lookAt(glm::vec3(0, 9, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1));
	projectionMatrix = glm::ortho(-9.0f, 9.0f, -9.0f, 9.0f, 0.01f, 10.0f);
	drawWeaponsStats();

	//Afisare numar de oponenti doborati atunci cand se modifica.
	if (numberOfEnemiesKilled != lastNumberOfEnemiesKilled) {
		if (numberOfEnemiesKilled == 1) {
			cout << "A fost doborat un inamic.\n";
		}
		else {
			cout << "Au fost doborati " << numberOfEnemiesKilled << " inamici.\n";
		}
		lastNumberOfEnemiesKilled = numberOfEnemiesKilled;
	}
}

void Tema2::FrameEnd(){}

void Tema2::RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix)
{
	if (!mesh || !shader)
		return;

	//Render an object using the specified shader and the specified position.
	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, false, glm::value_ptr(view));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, false, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	mesh->Render();
}

void Tema2::OnInputUpdate(float deltaTime, int mods)
{
	if(!gameOver){
		float cameraSpeed = 0.02f;
		float rad = glm::sqrt(glm::pow(camera->forward.x, 2) + glm::pow(camera->forward.z, 2));
		float cos = camera->forward.z / rad;
		float sin = camera->forward.x / rad;

		//Mers in fata.
		if (window->KeyHold(GLFW_KEY_W)) {
			cameraTP->MoveForward(cameraSpeed);
			cameraFP->MoveForward(cameraSpeed);
			steve.position.x += sin * cameraSpeed;
			steve.position.z += cos * cameraSpeed;
		}

		//Mers lateral stanga.
		if (window->KeyHold(GLFW_KEY_A)) {
			cameraTP->TranslateRight(-cameraSpeed);
			cameraFP->TranslateRight(-cameraSpeed);
			steve.position.x += cos * cameraSpeed;
			steve.position.z += -sin * cameraSpeed;
		}

		//Mers in spate.
		if (window->KeyHold(GLFW_KEY_S)) {
			cameraTP->MoveForward(-cameraSpeed);
			cameraFP->MoveForward(-cameraSpeed);
			steve.position.x += -sin * cameraSpeed;
			steve.position.z += -cos * cameraSpeed;
		}

		//Mers lateral dreapta.
		if (window->KeyHold(GLFW_KEY_D)) {
			cameraTP->TranslateRight(cameraSpeed);
			cameraFP->TranslateRight(cameraSpeed);
			steve.position.x += -cos * cameraSpeed;
			steve.position.z += sin * cameraSpeed;
		}
	}
}

void Tema2::OnKeyPress(int key, int mods)
{
	//Comutare mod pistol.
	if (key == GLFW_KEY_1 && !gameOver)
	{
		pistol = true;
		shotgun = false;
		grenadeLauncher = false;
		camera = cameraTP;
	}

	//Comutare mod pusca.
	if (key == GLFW_KEY_2 && !gameOver)
	{
		pistol = false;
		shotgun = true;
		grenadeLauncher = false;
		camera = cameraFP;
	}

	//Comutare mod aruncator de grenade.
	if (key == GLFW_KEY_3 && !gameOver)
	{
		pistol = false;
		shotgun = false;
		grenadeLauncher = true;
		camera = cameraTP;
	}
	
	//Iesire din joc.
	if (key == GLFW_KEY_ESCAPE) {
		exit(0);
	}
}

void Tema2::OnKeyRelease(int key, int mods){}

void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT) && !gameOver)
	{
		//Rotirea camerei si a jucatorului in jurul axei OY.
		float sensivityOY = 0.0005f;
		cameraTP->RotateThirdPerson_OY(-deltaX * sensivityOY);
		cameraFP->RotateFirstPerson_OY(-deltaX * sensivityOY);
		steve.rotation += -deltaX * sensivityOY;
	}
}

void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_2 && !gameOver) {
		//Determinarea pozitiei din care se lanseaza proiectilul si a directiei pe care o va avea.
		float thisMoment = Engine::GetElapsedTime();
		glm::vec3 direction = camera->forward;
		direction.y = 0;
		glm::vec3 startPosition = steve.position;
		startPosition.y = 0.5;
		
		//Lansare glont pistol.
		if (pistol && thisMoment - lastPistolShot >= pistolCooldown) {
			//Playerul are sansa sa lanseze un mega pistol bullet.
			int chance = rand() % chanceInterval;
			float power = pistolPower;
			if (chance == 0) {
				power *= chanceFactor;
				cout << "Steve a lansat un mega pistol bullet.\n";
			}
			Bullet aux = Bullet(startPosition, glm::normalize(direction), steve.rotation, power, pistolMaxDistance, "pistol", pistolSpeed);
			bullets.push_back(aux);
			lastPistolShot = thisMoment;
			numberOfShots++;
		}

		//Lansare glont pusca.
		if (shotgun && thisMoment - lastShotgunShot >= shotgunCooldown) {
			//Playerul are sansa sa lanseze un mega shotgun bullet.
			int chance = rand() % chanceInterval;
			float power = shotgunPower;
			if (chance == 0) {
				power *= chanceFactor;
				cout << "Steve a lansat un mega shotgun bullet.\n";
			}
			Bullet aux = Bullet(startPosition, glm::normalize(direction), steve.rotation, power, shotgunMaxDistance, "shotgun", shotgunSpeed);
			bullets.push_back(aux);
			lastShotgunShot = thisMoment;
			numberOfShots++;
		}

		//Lansare grenada.
		if (grenadeLauncher && thisMoment - lastGrenadeLauncherShot >= grenadeLauncherCooldown) {
			//Playerul are sansa sa lanseze o mega grenada.
			int chance = rand() % chanceInterval;
			float power = grenadeLauncherPower;
			if (chance == 0) {
				power *= chanceFactor;
				cout << "Steve a lansat o mega grenada.\n";
			}
			Grenade aux = Grenade(startPosition, glm::normalize(direction), steve.rotation, power, grenadeLauncherMaxDistance, grenadeLauncherSpeed);
			grenades.push_back(aux);
			lastGrenadeLauncherShot = thisMoment;
			numberOfShots++;
		}
	}
}

void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods){}

void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY){}

void Tema2::OnWindowResize(int width, int height){
	/*Refacerea minimap-ului si a timpilor de reincarcare ai
	armelor in cazul in care fereastra este redimensionata.*/
	int minimum = width < height ? width : height;
	viewSpaceMiniMap.x = width - minimum / 3 - width / 100;
	viewSpaceMiniMap.y = height / 100;
	viewSpaceMiniMap.width = minimum / 3;
	viewSpaceMiniMap.height = minimum / 3;
	viewSpaceWeaponsStats.x = width / 100;
	viewSpaceWeaponsStats.y = height / 100;
	viewSpaceWeaponsStats.width = minimum / 3;
	viewSpaceWeaponsStats.height = minimum / 9;
}
