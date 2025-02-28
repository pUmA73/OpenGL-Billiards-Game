#include <windows.h>  // biblioteci care urmeaza sa fie incluse
#include <stdlib.h> // necesare pentru citirea shader-elor
#include <stdio.h>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <GL/glew.h> // glew apare inainte de freeglut
#include <GL/freeglut.h> // nu trebuie uitat freeglut.h
#include "loadShaders.h"

#include "glm/glm.hpp"		
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>

#define WinHeight 600
#define WinWidth 1200
#define BallRadius 14.4f

// Clase generale
// --------------
class Ball
{
private:
	glm::vec2 position;
	glm::vec2 velocity;
	glm::vec2 acceleration;
	glm::vec4 color;
	float radius, mass;
	bool isPocketed;
	float animationDuration;
	static inline int idCounter = 0;
	int id;

public:
	// Constructors
	Ball(glm::vec2 position, glm::vec2 velocity, glm::vec4 color, float radius) :
		position(position), velocity(velocity), color(color), radius(radius), id(idCounter++)
	
	{
		this->acceleration = glm::vec2(0.0f, 0.0f);
		this->mass = radius * 10;
		this->isPocketed = false;
		this->animationDuration = 0.0f;
	}

	Ball(glm::vec2 position, glm::vec2 velocity, float radius) :
		position(position), velocity(velocity), radius(radius), id(idCounter++)
	{
		this->acceleration = glm::vec2(0.0f, 0.0f);
		this->color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		mass = radius * 10;
		isPocketed = false;
		this->animationDuration = 0.0f;
	}

	Ball(glm::vec2 position, glm::vec4 color, float radius) :
		position(position), color(color), radius(radius), id(idCounter++)
	{
		this->acceleration = glm::vec2(0.0f, 0.0f);
		this->velocity = glm::vec2(0.0f, 0.0f);
		mass = radius * 10;
		isPocketed = false;
		this->animationDuration = 0.0f;
	}

	Ball(glm::vec2 position, glm::vec2 velocity) : position(position), velocity(velocity), id(idCounter++)
	{
		this->acceleration = glm::vec2(0.0f, 0.0f);
		this->color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		this->radius = 0.0f;
		mass = radius * 10;
		isPocketed = false;
		this->animationDuration = 0.0f;
	}

	Ball(glm::vec2 position, float radius) : position(position), radius(radius), id(idCounter++)
	{
		this->acceleration = glm::vec2(0.0f, 0.0f);
		this->color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		this->velocity.x = 0.0f;
		this->velocity.y = 0.0f;
		mass = radius * 10;
		isPocketed = false;
		this->animationDuration = 0.0f;
	}

	Ball(glm::vec2 position) : position(position), id(idCounter++)
	{
		this->acceleration = glm::vec2(0.0f, 0.0f);
		this->velocity.x = 0.0f;
		this->velocity.y = 0.0f;
		this->color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		this->radius = 0.0f;
		mass = radius * 10;
		isPocketed = false;
		this->animationDuration = 0.0f;
	}

	// Setters
	void setPosition(const glm::vec2 position)
	{
		this->position = position;
	}

	void setPosition(const float x, const float y)
	{
		this->position.x = x;
		this->position.y = y;
	}

	void setAcceleration(const glm::vec2 acceleration)
	{
		this->acceleration = acceleration;
	}

	void setAcceleration(const float ax, const float ay)
	{
		this->acceleration.x = ax;
		this->acceleration.y = ay;
	}

	void setVelocity(const glm::vec2 velocity)
	{
		this->velocity = velocity;
	}

	void setVelocity(const float vx, const float vy)
	{
		this->velocity.x = vx;
		this->velocity.y = vy;
	}

	void setColor(const glm::vec4 color)
	{
		this->color = color;
	}

	void setColor(const float r, const float g, const float b, const float a)
	{
		this->color = glm::vec4(r, g, b, a);
	}

	void setRadius(const float radius)
	{
		this->radius = radius;
	}

	void setMass(const float mass)
	{
		this->mass = mass;
	}

	void setPocketed(const bool pocketed)
	{
		this->isPocketed = pocketed;
	}

	void setAnimationDuration(const float animationDuration)
	{
		this->animationDuration = animationDuration;
	}

	// Getters
	glm::vec2 getPosition()
	{
		return this->position;
	}

	glm::vec2 getAcceleration()
	{
		return this->acceleration;
	}

	glm::vec2 getVelocity()
	{
		return this->velocity;
	}

	glm::vec4 getColor()
	{
		return this->color;
	}

	float getRadius()
	{
		return this->radius;
	}

	int getId()
	{
		return this->id;
	}

	float getMass()
	{
		return this->mass;
	}

	bool getPocketed()
	{
		return this->isPocketed;
	}

	float getAnimationDuration()
	{
		return this->animationDuration;
	}
};

//////////////////////////////////////

GLuint
VaoId,
VboId,
EboId,
VaoId2,
VboId2, 
EboId2,
positionVBO,
ColorBufferId,
ColorBufferId2,
ProgramId;

GLuint modelMatLocation, ballColorLocation, codCol, codColLocation;

std::vector<Ball> balls;
std::vector<std::pair<Ball*, Ball*>> collidingBalls;
std::vector <Ball*> fakeBalls;
std::vector<Ball> pockets;
std::vector<std::pair<glm::vec2, glm::vec2>> edges;
std::vector<std::pair<glm::vec2, glm::vec2>> mantles;

bool rightMouseDown = 0, showDirectionLine;
glm::vec2 currMousePos;

Ball* selectedBall = nullptr;

//	Variabile pentru proiectia ortogonala;
float aMin = -600, aMax = 600, bMin = -300, bMax = 300;
float
deltaA = aMax - aMin, deltaB = bMax - bMin, // lungimile laturilor dreptunghiului decupat
centerA = (aMin + aMax) * 0.5, centerB = (bMin + bMax) * 0.5; // centrul dreptunghiului decupat
glm::mat4 resizeMatrix;

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

glm::vec2 CircleLineIntersect(const float cx, const float cy, const float x1, const float y1, const float x2, const float y2, const float r)
{
	// testam pentru buzunare - ar trebui sa existe un singur punct de intersectie 
	
	// linie verticala
	if (x1 == x2)
	{
		float dx = x1 - cx;
		float delta = r * r - dx * dx;
		if (delta > 0)
		{
			delta = sqrt(delta);
			float yy1 = cy + delta;
			float yy2 = cy - delta;

			if (yy1 >= std::min(y1, y2) && yy1 <= std::max(y1, y2))
				return glm::vec2(x1, yy1);
			else
				return glm::vec2(x1, yy2);
		}
	}
	else	// linie orizontala
	{
		float dy = y1 - cy;
		float delta = r * r - dy * dy;
		if (delta > 0)
		{
			delta = sqrt(delta);
			float xx1 = cx + delta;
			float xx2 = cx - delta;

			if (xx1 >= std::min(x1, x2) && xx1 <= std::max(x1, x2))
				return glm::vec2(xx1, y1);
			else
				return glm::vec2(xx2, y1);
		}
	}

	// daca nu gasim intersectie returnam originea
	return glm::vec2(0.0f, 0.0f);
}

void InitializeBalls(float radius)
{
	float spacing = 1.0f;
	
	float startX = -100.0f;
	float startY = 0.0f;

	int rows[] = { 1, 2, 3, 4, 5 };

	// pozitia initiala pentru bila alba
	balls.emplace_back(glm::vec2(0.6f, 0.0f), radius);
	//balls[0].setVelocity(glm::vec2(-1.0f, 0.0f));

	// calculam pozitiile initiale pentru bilele colorate 
	for (int i = 0; i < 5; ++i)
	{
		float rowX = startX - i * (2 * radius + spacing);
		if (rows[i] % 2 == 0)
		{
			for (int j = 1; j <= rows[i] / 2; j++)
			{
				float posY = startY + j * (radius + spacing) + radius * (j - 1);
				balls.emplace_back(glm::vec2(rowX, posY), radius);
				balls.emplace_back(glm::vec2(rowX, -posY), radius);
			}
		}
		else
		{
			balls.emplace_back(glm::vec2(rowX, 0.0f), radius);
			for (int j = 1; j <= rows[i] / 2; j++)
			{
				float posY = startY + j * (2 * radius + spacing);
				balls.emplace_back(glm::vec2(rowX, posY), radius);
				balls.emplace_back(glm::vec2(rowX, -posY), radius);
			}
		}
	}

	// culorile initiale pentru bile
	balls[0].setColor(1.0f, 1.0f, 1.0f, 1.0f);
	balls[1].setColor(1.0f, 0.843f, 0.0f, 1.0f);
	balls[2].setColor(0.0f, 0.0f, 1.0f, 1.0f);
	balls[3].setColor(1.0f, 0.0f, 0.0f, 1.0f);
	balls[4].setColor(0.0f, 0.0f, 0.0f, 1.0f);
	balls[5].setColor(1.0f, 0.647f, 0.0f, 1.0f);
	balls[6].setColor(0.0f, 0.502f, 0.0f, 1.0f);
	balls[7].setColor(0.502f, 0.0f, 0.0f, 1.0f);
	balls[8].setColor(0.502f, 0.0f, 0.502f, 1.0f);
	balls[9].setColor(1.0f, 0.843f, 0.0f, 1.0f);
	balls[10].setColor(0.0f, 0.0f, 1.0f, 1.0f);
	balls[11].setColor(1.0f, 0.0f, 0.0f, 1.0f);
	balls[12].setColor(0.502f, 0.0f, 0.502f, 1.0f);
	balls[13].setColor(1.0f, 0.647f, 0.0f, 1.0f);
	balls[14].setColor(0.0f, 0.502f, 0.0f, 1.0f);
	balls[15].setColor(0.502f, 0.0f, 0.0f, 1.0f);
}

void InitializePockets()
{
	glm::vec2 auxVec;

	float pocketRadius = BallRadius * 1.5f;

	// gaura stanga jos
	auxVec = glm::vec2(-520.0f + 500.0f, -270.0f + 250.0f);
	auxVec = glm::normalize(auxVec);
	pockets.emplace_back(glm::vec2(-500.0f, -250.0f) - 15.0f * auxVec, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), pocketRadius);

	// gaura dreapta jos
	auxVec = glm::vec2(520.0f - 500.0f, -270.0f + 250.0f);
	auxVec = glm::normalize(auxVec);
	pockets.emplace_back(glm::vec2(500.0f, -250.0f) - 15.0f * auxVec, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), pocketRadius);

	// gaura drapta sus
	auxVec = glm::vec2(520.0f - 500.0f, 270.0f - 250.0f);
	auxVec = glm::normalize(auxVec);
	pockets.emplace_back(glm::vec2(500.0f, 250.0f) - 15.0f * auxVec, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), pocketRadius);

	// gaura stanga sus
	auxVec = glm::vec2(-520.0f + 500.0f, 270.0f - 250.0f);
	auxVec = glm::normalize(auxVec);
	pockets.emplace_back(glm::vec2(-500.0f, 250.0f) - 15.0f * auxVec, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), pocketRadius);

	// gaura sus mijloc
	auxVec = glm::vec2(0.0f, 1.0f);
	auxVec = glm::normalize(auxVec);
	pockets.emplace_back(glm::vec2(0.0f, 250.0f) + BallRadius / 2 * auxVec, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), pocketRadius);

	// gaura jos mijloc
	auxVec = glm::vec2(0.0f, -1.0f);
	auxVec = glm::normalize(auxVec);
	pockets.emplace_back(glm::vec2(0.0f, -250.0f) + BallRadius / 2 * auxVec, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f), pocketRadius);
}

void InitializeMantles()
{
	std::vector<std::pair<float, float>> intersectionPoints;
	float pocketRadius = BallRadius * 1.5f;
	glm::vec2 auxVec;
	
	glm::vec2 shMantle1, shMantle2, shMantle3, shMantle4;
	glm::vec2 lgMantle1, lgMantle2;

	shMantle1 = CircleLineIntersect(pockets[0].getPosition().x, pockets[0].getPosition().y, -500.0f, -250.0f, 0.0f, -250.0f, pocketRadius);
	shMantle2 = CircleLineIntersect(pockets[5].getPosition().x, pockets[5].getPosition().y, -500.0f, -250.0f, 0.0f, -250.0f, pocketRadius);

	shMantle3 = CircleLineIntersect(pockets[5].getPosition().x, pockets[5].getPosition().y, 0.0f, -250.0f, 500.0f, -250.0f, pocketRadius);
	shMantle4 = CircleLineIntersect(pockets[1].getPosition().x, pockets[1].getPosition().y, 0.0f, -250.0f, 500.0f, -250.0f, pocketRadius);

	lgMantle1 = CircleLineIntersect(pockets[0].getPosition().x, pockets[0].getPosition().y, -500.0f, -250.0f, -500.0f, 250.0f, pocketRadius);
	lgMantle2 = CircleLineIntersect(pockets[3].getPosition().x, pockets[3].getPosition().y, -500.0f, -250.0f, -500.0f, 250.0f, pocketRadius);
	
	mantles.push_back(std::make_pair(shMantle1, shMantle2));
	mantles.push_back(std::make_pair(glm::vec2(shMantle2.x, -shMantle2.y), glm::vec2(shMantle1.x, -shMantle1.y)));
	mantles.push_back(std::make_pair(shMantle3, shMantle4));
	mantles.push_back(std::make_pair(glm::vec2(shMantle4.x, -shMantle4.y), glm::vec2(shMantle3.x, -shMantle3.y)));
	mantles.push_back(std::make_pair(lgMantle1, lgMantle2));
	mantles.push_back(std::make_pair(glm::vec2(-lgMantle2.x, lgMantle2.y), glm::vec2(-lgMantle1.x, lgMantle1.y)));

	for (auto& mantle : mantles)
	{
		if (mantle.first.y == mantle.second.y)
		{
			edges.push_back(std::make_pair(mantle.first, glm::vec2(mantle.first.x - 5.0f * sgn(mantle.first.y), mantle.first.y - 10.0f * sgn(mantle.first.y))));
			edges.push_back(std::make_pair(glm::vec2(mantle.first.x - 5.0f * sgn(mantle.first.y), mantle.first.y - 10.0f * sgn(mantle.first.y)), 
				glm::vec2(mantle.second.x + 5.0f * sgn(mantle.second.y), mantle.second.y - 10.0f * sgn(mantle.second.y))));
			edges.push_back(std::make_pair((glm::vec2(mantle.second.x + 5.0f * sgn(mantle.second.y), mantle.second.y - 10.0f * sgn(mantle.second.y))), mantle.second));
		}
		else
		{
			edges.push_back(std::make_pair(mantle.first, glm::vec2(mantle.first.x - 10.0f * sgn(mantle.first.x), mantle.first.y - 5.0f * sgn(mantle.first.x))));
			edges.push_back(std::make_pair(glm::vec2(mantle.first.x - 10.0f * sgn(mantle.first.x), mantle.first.y - 5.0f * sgn(mantle.first.x)), 
				glm::vec2(mantle.second.x - 10.0f * sgn(mantle.second.x), mantle.second.y + 5.0f * sgn(mantle.second.x))));
			edges.push_back(std::make_pair(glm::vec2(mantle.second.x - 10.0f * sgn(mantle.second.x), mantle.second.y + 5.0f * sgn(mantle.second.x)), mantle.second));
		}
	}
}

void HandlePocketedBalls()
{
	auto IsBallInPocket = [](glm::vec2 pocketPos, glm::vec2 ballPos, const float pocketRad)
		{
			// distanta de la centrul bilei la centrul gaurii este mai mica decat raza gaurii
			return glm::length(pocketPos - ballPos) <= pocketRad;
		};

	for (auto& ball : balls)
		for (auto& pocket : pockets)
		{
			if (IsBallInPocket(pocket.getPosition(), ball.getPosition(), pocket.getRadius()))
			{
				if (ball.getColor() == glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
				{
					ball.setPosition(200.0f, 0.0f);
					ball.setVelocity(0.0f, 0.0f);
				}
				else
				{
					ball.setPocketed(true);
					// mutam bila pentru a nu interfera cu restul bilelor de pe tabla,
					// pentru ca bila nu va mai fi randata, dar va fi prezenta ca obiect
					ball.setPosition(-1100.0f, -570.0f);
				}
				break;
			}
		}
}

void HandleCollisions()
{
	/*float elapsedTime = 0.1f;

	for (auto& ball : balls)
	{
		ball.setVelocity(ball.getAcceleration() * elapsedTime);
		ball.setPosition(ball.getVelocity() * elapsedTime);

		if (glm::length(ball.getVelocity()) < 0.01f)
		{
			ball.setVelocity(0.0f, 0.0f);
		}
	}*/

	for (auto it = collidingBalls.begin(); it != collidingBalls.end();)
	{
		Ball* b1 = it->first;
		Ball* b2 = it->second;

		float distance = glm::length(b1->getPosition() - b2->getPosition());

		// Normala
		glm::vec2 normal;
		normal.x = (b2->getPosition().x - b1->getPosition().x) / distance;
		normal.y = (b2->getPosition().y - b1->getPosition().y) / distance;

		// Tangentiala
		glm::vec2 tangential;
		tangential.x = -normal.y;
		tangential.y = normal.x;

		// Dot product tangential
		float dpTangential1, dpTangential2;
		dpTangential1 = b1->getVelocity().x * tangential.x + b1->getVelocity().y * tangential.y;
		dpTangential2 = b2->getVelocity().x * tangential.x + b2->getVelocity().y * tangential.y;

		// Dot product normal
		float dpNormal1, dpNormal2;
		dpNormal1 = b1->getVelocity().x * normal.x + b1->getVelocity().y * normal.y;
		dpNormal2 = b2->getVelocity().x * normal.x + b2->getVelocity().y * normal.y;

		float mass1 = b1->getMass();
		float mass2 = b2->getMass();
		float mom1 = (dpNormal1 * (mass1 - mass2) + 2.0f * mass2 * dpNormal2) / (mass1 + mass2);
		float mom2 = (dpNormal2 * (mass2 - mass1) + 2.0f * mass1 * dpNormal1) / (mass1 + mass2);

		b1->setVelocity(tangential.x * dpTangential1 + normal.x * mom1, tangential.y * dpTangential1 + normal.y * mom1);
		b2->setVelocity(tangential.x * dpTangential2 + normal.x * mom2, tangential.y * dpTangential2 + normal.y * mom2);
	
		it = collidingBalls.erase(it);
	}
	for (auto& fakeBall : fakeBalls)
		delete fakeBall;
	fakeBalls.clear();
}

void DoBallsCollide()
{
	auto DoBallsOverlap = [](glm::vec2 pos1, float r1, glm::vec2 pos2, float r2)
		{
			glm::vec2 direction = pos2 - pos1;
			float distance = glm::length(direction);

			if (distance <= r1 + r2)
				return distance - (r1 + r2);
			else
				return -1.0f;
		};

	float collisionBuffer = 0.1f;

	for (auto& ball : balls)
	{
		// verificare cu mante
		for (auto& edge : edges)
		{
			glm::vec2 lineVec = edge.second - edge.first;
			glm::vec2 ballLineVec = ball.getPosition() - edge.first;

			float length = glm::length(lineVec);
			float t = std::max(0.0f, std::min(length * length, (lineVec.x * ballLineVec.x + lineVec.y * ballLineVec.y))) / (length * length);

			glm::vec2 closePoint = edge.first + t * lineVec;
			float dist = glm::length(ball.getPosition() - closePoint);

			if (dist <= BallRadius)
			{
				//std::cout << edge.first.x << " " << edge.first.y << " " << edge.second.x << " " << edge.second.y << "\n";
				//std::cout << dist << "\n";

				Ball* fakeBall = new Ball(closePoint);
				fakeBall->setRadius(0.1f);
				fakeBall->setMass(1.0f * ball.getMass());
				fakeBall->setVelocity(-ball.getVelocity());

				fakeBalls.push_back(fakeBall);
				collidingBalls.push_back({ &ball, fakeBall });

				float overlap = ball.getRadius() - dist + collisionBuffer;

				// Mutam bila curenta
				ball.setPosition(ball.getPosition() + overlap * glm::normalize(ball.getPosition() - closePoint));
				/*glm::vec2 normal = glm::normalize(ball.getPosition() - closePoint);
				glm::vec2 velocity = ball.getVelocity();

				ball.setVelocity(velocity - 1.2f * glm::dot(velocity, normal) * normal);*/
			}
		}

		// verificare cu alte bile
		for (auto& target : balls)
		{
			if (ball.getId() != target.getId())
			{
				float doOverlap = DoBallsOverlap(ball.getPosition(), ball.getRadius(), target.getPosition(), target.getRadius());

				if (doOverlap != -1)
				{	
					// Introducem bilele participante la coliziune in vectorul de coliziuni
					collidingBalls.push_back({ &ball, &target });

					float overlap = 0.5f * doOverlap;
					
					// Mutam bila curenta
					ball.setPosition(ball.getPosition() - overlap * glm::normalize(ball.getPosition() - target.getPosition()));
					// Mutam bila tinta
					target.setPosition(target.getPosition() + overlap * glm::normalize(ball.getPosition() - target.getPosition()));
				}
			}
		}
	}

	HandleCollisions();
	HandlePocketedBalls();
}

void CreateVBO(const float rad)
{

	float centerX = 0.0f, centerY = 0.0f;

	const int steps = 25;

	std::vector<GLfloat> Vertices;
	std::vector<GLfloat> Colors;

	// Calculam punctele pentru cerc
	Vertices.insert(Vertices.end(), { centerX, centerY, 0.0f, 1.0f });

	for (int i = 0; i <= steps; ++i)
	{
		float angle = 2.0f * M_PI * i / steps;
		float newX = centerX + rad * cos(angle);
		float newY = centerY + rad * sin(angle);

		Vertices.insert(Vertices.end(), { newX, newY, 0.0f, 1.0f });
	}

	for (int i = 0; i < Vertices.size() / 4; ++i)
		Colors.insert(Colors.end(), { 0.0f, 0.0f, 0.0f, 1.0f });

	std::vector<GLuint> Indices;
	Indices.push_back(0); // Center point
	for (int i = 1; i <= steps + 1; ++i)
	{
		Indices.push_back(i);
	}

	// se creeaza un buffer nou
	glGenBuffers(1, &VboId);
	// este setat ca buffer curent
	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	// varfurile sunt "copiate" in bufferul curent
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(GLfloat), Vertices.data(), GL_STATIC_DRAW);

	// se creeaza / se leaga un VAO (Vertex Array Object) - util cand se utilizeaza mai multe VBO
	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);
	// se activeaza lucrul cu atribute; atributul 0 = pozitie
	glEnableVertexAttribArray(0);
	// 
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	// un nou buffer, pentru culoare
	glGenBuffers(1, &ColorBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
	glBufferData(GL_ARRAY_BUFFER, Colors.size() * sizeof(GLfloat), Colors.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &EboId);					//  Generarea bufferului si indexarea acestuia catre variabila EboId;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(GLuint), Indices.data(), GL_STATIC_DRAW);

	// atributul 1 =  culoare
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL) ;
}

void CreateVBOTable()
{
	std::vector<GLfloat> vertices =
	{
		// Punctele pentru zona jucabila
		-500.0f, -250.0f, 0.0f, 1.0f,
		 500.0f, -250.0f, 0.0f, 1.0f,
		 500.0f,  250.0f, 0.0f, 1.0f,
		-500.0f,  250.0f, 0.0f, 1.0f,

		// Margina stanga
		-520.0f, -270.0f, 0.0f, 1.0f,
		-500.0f, -270.0f, 0.0f, 1.0f,
		-500.0f,  270.0f, 0.0f, 1.0f,
		-520.0f,  270.0f, 0.0f, 1.0f,

		// Margine jos
		-500.0f, -270.0f, 0.0f, 1.0f,
		 500.0f, -270.0f, 0.0f, 1.0f,
		 500.0f, -250.0f, 0.0f, 1.0f,
		-500.0f, -250.0f, 0.0f, 1.0f,

		// Margine dreapta
		 500.0f, -270.0f, 0.0f, 1.0f,
		 520.0f, -270.0f, 0.0f, 1.0f,
		 520.0f,  270.0f, 0.0f, 1.0f,
		 500.0f,  270.0f, 0.0f, 1.0f,

		// Margine sus
		-500.0f,  250.0f, 0.0f, 1.0f,
		 500.0f,  250.0f, 0.0f, 1.0f,
		 500.0f,  270.0f, 0.0f, 1.0f,
		-500.0f,  270.0f, 0.0f, 1.0f,
	};

	// Adaugam punctele pentru mante
	for (auto& mantle : mantles)
	{
		if (mantle.first.y == mantle.second.y)
		{
			vertices.insert(vertices.end(), { mantle.first.x, mantle.first.y, 0.0f, 1.0f });
			vertices.insert(vertices.end(), { mantle.second.x, mantle.second.y, 0.0f, 1.0f });
			vertices.insert(vertices.end(), { mantle.second.x + 5.0f * sgn(mantle.second.y), mantle.second.y - 10.0f * sgn(mantle.second.y), 0.0f, 1.0f});
			vertices.insert(vertices.end(), { mantle.first.x - 5.0f * sgn(mantle.first.y), mantle.first.y - 10.0f * sgn(mantle.first.y), 0.0f, 1.0f});
		}
		else
		{
			vertices.insert(vertices.end(), { mantle.first.x, mantle.first.y, 0.0f, 1.0f });
			vertices.insert(vertices.end(), { mantle.first.x - 10.0f * sgn(mantle.first.x), mantle.first.y - 5.0f * sgn(mantle.first.x), 0.0f, 1.0f});
			vertices.insert(vertices.end(), { mantle.second.x - 10.0f * sgn(mantle.second.x), mantle.second.y + 5.0f * sgn(mantle.second.x), 0.0f, 1.0f});
			vertices.insert(vertices.end(), { mantle.second.x, mantle.second.y, 0.0f, 1.0f });
		}
	}

	std::vector<GLfloat> colors =
	{
		// Culoare pentru zona jucabila
		0.2f, 0.5f, 0.3f, 1.0f,
		0.2f, 0.5f, 0.3f, 1.0f,
		0.2f, 0.5f, 0.3f, 1.0f,
		0.2f, 0.5f, 0.3f, 1.0f,
		
		// Culoare pentru margini
		0.71f, 0.40f, 0.16f, 1.0f,
		0.71f, 0.40f, 0.16f, 1.0f,
		0.71f, 0.40f, 0.16f, 1.0f,
		0.71f, 0.40f, 0.16f, 1.0f,

		0.71f, 0.40f, 0.16f, 1.0f,
		0.71f, 0.40f, 0.16f, 1.0f,
		0.71f, 0.40f, 0.16f, 1.0f,
		0.71f, 0.40f, 0.16f, 1.0f,

		0.71f, 0.40f, 0.16f, 1.0f,
		0.71f, 0.40f, 0.16f, 1.0f,
		0.71f, 0.40f, 0.16f, 1.0f,
		0.71f, 0.40f, 0.16f, 1.0f,

		0.71f, 0.40f, 0.16f, 1.0f,
		0.71f, 0.40f, 0.16f, 1.0f,
		0.71f, 0.40f, 0.16f, 1.0f,
		0.71f, 0.40f, 0.16f, 1.0f,
	};

	// Adaugam culorile pentru mante
	int cSize = colors.size();
	for (int i = 0; i < (vertices.size() - cSize); ++i)
	{
		colors.insert(colors.end(), { 0.2f, 0.7f, 0.4f, 1.0f, });
	}

	std::vector<GLuint> indices;
	for (int i = 0; i < 11 * 4; i += 4)
	{
		indices.push_back(i);
		indices.push_back(i + 1);
		indices.push_back(i + 2);
		indices.push_back(i + 3);
		indices.push_back(i);
		indices.push_back(i + 2);
	}
	
	glGenVertexArrays(1, &VaoId2);         //  Generarea VAO si indexarea acestuia catre variabila VaoId2;
	glBindVertexArray(VaoId2);

	//  Se creeaza un buffer pentru VARFURI;
	glGenBuffers(1, &VboId2);                                                        //  Generarea bufferului si indexarea acestuia catre variabila VboId2;
	glBindBuffer(GL_ARRAY_BUFFER, VboId2);                                           //  Setarea tipului de buffer - atributele varfurilor;
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
	//  Se asociaza atributul (0 = coordonate) pentru shader;
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//  Se creeaza un buffer pentru CULOARE;
	glGenBuffers(2, &ColorBufferId2);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId2);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(GLfloat), colors.data(), GL_STATIC_DRAW);
	//  Se asociaza atributul (1 =  culoare) pentru shader;
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//	Se creeaza un buffer pentru INDICI;
	glGenBuffers(3, &EboId2);														//  Generarea bufferului si indexarea acestuia catre variabila EboId2;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}

void DestroyVBO(void)
{
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VboId);
	glDeleteBuffers(1, &ColorBufferId);
	glDeleteBuffers(1, &EboId);

	glDeleteBuffers(1, &VboId2);
	glDeleteBuffers(1, &ColorBufferId2);
	glDeleteBuffers(1, &EboId2);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);
	glDeleteVertexArrays(1, &VaoId2);
}

void CreateShaders(void)
{
	ProgramId = LoadShaders("example.vert", "example.frag");
	glUseProgram(ProgramId);
}
void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
}

void Initialize(void)
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // culoarea de fond a ecranului
	InitializeBalls(14.4f);
	InitializePockets();
	InitializeMantles();
	CreateVBOTable();
	CreateVBO(14.4f);
	CreateShaders();

	modelMatLocation = glGetUniformLocation(ProgramId, "modelMatrix");
	ballColorLocation = glGetUniformLocation(ProgramId, "ballColor");
	codColLocation = glGetUniformLocation(ProgramId, "codCol");

	/*glUniform2f(glGetUniformLocation(ProgramId, "lightPos"), 0.0f, 0.0f);  
	glUniform3f(glGetUniformLocation(ProgramId, "lightColor"), 1.0f, 1.0f, 1.0f);  */

	for (auto edge : edges)
	{
		std::cout << edge.first.x << " " << edge.first.y << " " << edge.second.x << " " << edge.second.y << "\n";
	}

	resizeMatrix = glm::ortho(aMin, aMax, bMin, bMax);
}
void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT);       

	//glDrawElements(GL_TRIANGLE_FAN, 27, GL_UNSIGNED_INT, (void*)0);

	//DoBallsCollide();

	// Desenam masa
	codCol = 0;
	glUniform1i(codColLocation, codCol);
	glBindVertexArray(VaoId2);
	glUniformMatrix4fv(modelMatLocation, 1, GL_FALSE, glm::value_ptr(resizeMatrix));
	glDrawElements(GL_TRIANGLES, 11 * 6, GL_UNSIGNED_INT, (void*)0);

	// Desenam gaurile
	codCol = 1;
	glUniform1i(codColLocation, codCol);
	for (auto& pocket : pockets)
	{
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 scale = glm::mat4(1.0f);

		model = glm::translate(model, glm::vec3(pocket.getPosition(), 0.0f));
		scale = glm::scale(scale, glm::vec3(1.5f, 1.5f, 1.0f));

		model = resizeMatrix * model * scale;

		glUniformMatrix4fv(modelMatLocation, 1, GL_FALSE, glm::value_ptr(model));
		glUniform4fv(ballColorLocation, 1, glm::value_ptr(pocket.getColor()));

		glBindVertexArray(VaoId);
		glDrawElements(GL_TRIANGLE_FAN, 27, GL_UNSIGNED_INT, (void*)0);
	}

	// Desenam bilele
	for (auto& ball : balls)
		if(!ball.getPocketed())
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(ball.getPosition(), 0.0f));

			model = resizeMatrix * model;

			glUniformMatrix4fv(modelMatLocation, 1, GL_FALSE, glm::value_ptr(model));
			glUniform4fv(ballColorLocation, 1, glm::value_ptr(ball.getColor()));

			// desenam bila
			glBindVertexArray(VaoId);
			glDrawElements(GL_TRIANGLE_FAN, 27, GL_UNSIGNED_INT, (void*)0);
		}

	glutSwapBuffers();
	glFlush();
}
void Cleanup(void)
{
	DestroyShaders();
	DestroyVBO();
}

void ConvertToOpenGLCoords(float x, float y, float& opx, float& opy)
{
	opx = (x / WinWidth) * 1200.0f - 600.0f;

	opy = 300.0f - (y / WinHeight) * 600.0f;
}

void UseMouse(int button, int state, int x, int y)
{	
	auto IsInsideBall = [](float px, float py, Ball& ball)
	{
		float bx = ball.getPosition().x;
		float by = ball.getPosition().y;

		float rad = ball.getRadius();

		return (px - bx) * (px - bx) + (py - by) * (py - by) <= (rad * rad);
	};

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		float mx, my;
		ConvertToOpenGLCoords(x, y, mx, my);
		for (auto& ball : balls)
		{
			if (IsInsideBall(mx, my, ball))
			{
				selectedBall = &ball;
				break;
			}
		}
	}

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		rightMouseDown = true;
		float mx, my;
		ConvertToOpenGLCoords(x, y, mx, my);
		for (auto& ball : balls)
		{
			if (IsInsideBall(mx, my, ball))
			{
				selectedBall = &ball;
				showDirectionLine = true;
				break;
			}
		}
	}
	
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		if (selectedBall != nullptr)
		{
			selectedBall = nullptr;
		}
	}

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		rightMouseDown = false;
		if (selectedBall != nullptr)
		{
			selectedBall->setVelocity(5.0f * (selectedBall->getPosition() - currMousePos));
			selectedBall = nullptr;
			showDirectionLine = false;
		}
	}
}

void MouseMotion(int x, int y)
{
	if (selectedBall != nullptr)
	{
		float mx, my;
		ConvertToOpenGLCoords(x, y, mx, my);

		if (rightMouseDown)
		{
			currMousePos.x = mx;
			currMousePos.y = my;
		}
		else
		{
			selectedBall->setPosition(mx, my);
		}
		//std::cout << mx << " " << my << "\n";
		
		DoBallsCollide();

		glutPostRedisplay();
	}
}

void Update(int value)
{
	for (auto& ball : balls)
	{
		// modificam pozitia bilelor bazat pe viteza
		ball.setPosition(ball.getPosition() + ball.getVelocity() * 0.01f);

		// modificam viteza pentru a simula frecarea
		ball.setVelocity(ball.getVelocity() * 0.97f);
	}
	
	DoBallsCollide();

	glutPostRedisplay();
	glutTimerFunc(16, Update, 0);
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(100, 100); // pozitia initiala a ferestrei
	glutInitWindowSize(WinWidth, WinHeight); //dimensiunile ferestrei
	glutCreateWindow("Grafica pe calculator - primul exemplu"); // titlul ferestrei
	glewInit(); // nu uitati de initializare glew; trebuie initializat inainte de a a initializa desenarea
	Initialize();
	// Functia de randare
	glutDisplayFunc(RenderFunction);
	// Functia care se ocupa cu input-ul de la butoanele mouse-ului
	glutMouseFunc(UseMouse);
	// Functia care se ocupa cu input-ul de la miscarea mouse-ului
	glutMotionFunc(MouseMotion);
	// Functia pentru update - modifica pozitiile si viteza bilelor
	glutTimerFunc(16, Update, 0);

	glutCloseFunc(Cleanup);
	glutMainLoop();
}

