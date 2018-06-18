// Defines the entry point for the console application.
//
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <time.h>

#include "Object.h"
#include "Camera.h"
#include "Shader.h"

#include "vec.hpp"
#include "mat.hpp"
#include "transform.hpp"

void init();
void display();
void reshape(int, int);
void idle();
void keyboard(unsigned char, int, int);
void special(int, int, int);
void delay(float secs);
void createGLUTMenus();
void processMenuEvents(int option);
void myMouseFunc(int button, int state, int x, int y);

GLuint program;

GLint  loc_a_vertex;
GLint  loc_a_normal;

GLint  loc_u_pvm_matrix;
GLint  loc_u_view_matrix;
GLint  loc_u_model_matrix;
GLint  loc_u_normal_matrix;

GLint  loc_u_light_vector;

GLint  loc_u_light_ambient;
GLint  loc_u_light_diffuse;
GLint  loc_u_light_specular;

GLint  loc_u_material_ambient;
GLint  loc_u_material_diffuse;
GLint  loc_u_material_specular;
GLint  loc_u_material_shininess;

kmuvcl::math::mat4x4f   mat_PVM;

kmuvcl::math::vec4f light_vector      = kmuvcl::math::vec4f(10.0f, 10.0f, 10.0f);
kmuvcl::math::vec4f light_ambient     = kmuvcl::math::vec4f(1.0f, 1.0f, 1.0f, 1.0f);
kmuvcl::math::vec4f light_diffuse     = kmuvcl::math::vec4f(1.0f, 1.0f, 1.0f, 1.0f);
kmuvcl::math::vec4f light_specular    = kmuvcl::math::vec4f(1.0f, 1.0f, 1.0f, 1.0f);

std::string g_filename;
Object      g_model;        // object
Camera		  g_camera;											// viewer (you)

float model_scale = 1.0f;
float model_angle = 0.0f;

std::chrono::time_point<std::chrono::system_clock> prev, curr;

int main(int argc, char* argv[])
{
  if (argc > 1)
  {
    g_filename = argv[2];
  }
  else
  {
    g_filename = "./data/Hangar.obj";
  }

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(100, 100);
  glutInitWindowSize(640, 640);

  glutCreateWindow("Modeling & Navigating Your Studio");
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(special);
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(myMouseFunc);
  createGLUTMenus();
  glutIdleFunc(idle);

  if (glewInit() != GLEW_OK)
  {
      std::cerr << "failed to initialize glew" << std::endl;
      return -1;
  }

  init();

  glutMainLoop();

  return 0;
}

void createGLUTMenus(){
  GLint SubMenu1 = glutCreateMenu(processMenuEvents);
  glutAddMenuEntry("3",1 );
  glutAddMenuEntry("5",2 );
  glutAddMenuEntry("7",3 );

  GLint menu = glutCreateMenu(processMenuEvents);
  glutAddSubMenu("moveMuseum",SubMenu1);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void processMenuEvents(int option){
  switch (option){

  }
}

void delay(float secs)
{
	float end = clock() / CLOCKS_PER_SEC + secs;
	while ((clock() / CLOCKS_PER_SEC) < end);
}

void myMouseFunc(int button, int state, int x, int y)
{
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

	}
	else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP) {

	}
}


void init()
{
  g_model.load_simple_obj(g_filename);
  //g_model.print();

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);    // for filled polygon rendering

  glEnable(GL_DEPTH_TEST);

  program = Shader::create_program("./shader/phong_vert.glsl", "./shader/phong_frag.glsl");

  loc_u_pvm_matrix         = glGetUniformLocation(program, "u_pvm_matrix");
  loc_u_view_matrix        = glGetUniformLocation(program, "u_view_matrix");
  loc_u_model_matrix       = glGetUniformLocation(program, "u_model_matrix");
  loc_u_normal_matrix      = glGetUniformLocation(program, "u_normal_matrix");

  loc_u_light_vector       = glGetUniformLocation(program, "u_light_vector");

  loc_u_light_ambient      = glGetUniformLocation(program, "u_light_ambient");
  loc_u_light_diffuse      = glGetUniformLocation(program, "u_light_diffuse");
  loc_u_light_specular     = glGetUniformLocation(program, "u_light_specular");

  loc_u_material_ambient   = glGetUniformLocation(program, "u_material_ambient");
  loc_u_material_diffuse   = glGetUniformLocation(program, "u_material_diffuse");
  loc_u_material_specular  = glGetUniformLocation(program, "u_material_specular");
  loc_u_material_shininess = glGetUniformLocation(program, "u_material_shininess");

  loc_a_vertex             = glGetAttribLocation(program, "a_vertex");
  loc_a_normal             = glGetAttribLocation(program, "a_normal");

  prev = curr = std::chrono::system_clock::now();
}

void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(program);

  // Camera setting
  kmuvcl::math::mat4x4f   mat_Proj, mat_View_inv, mat_Model;

  // camera intrinsic param
  mat_Proj = kmuvcl::math::perspective(60.0f, 1.0f, 0.001f, 100.0f);

/*
  // camera extrinsic param
  mat_View_inv = kmuvcl::math::lookAt(
    0.0f, 0.0f, 3.0f,
    0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f);
*/

  // camera extrinsic param
  mat_View_inv= kmuvcl::math::lookAt(
  	g_camera.position()(0), g_camera.position()(1)+1.0f, g_camera.position()(2),				// eye position
  	g_camera.center_position()(0), g_camera.center_position()(1)+1.0f, g_camera.center_position()(2), // center position
  	g_camera.up_direction()(0), g_camera.up_direction()(1), g_camera.up_direction()(2)			// up direction
  	);

  mat_Model = kmuvcl::math::scale(model_scale, model_scale, model_scale);
  //mat_Model = kmuvcl::math::rotate(model_angle*0.7f, 0.0f, 0.0f, 1.0f) * mat_Model;
  //mat_Model = kmuvcl::math::rotate(model_angle,      0.0f, 1.0f, 0.0f) * mat_Model;
  //mat_Model = kmuvcl::math::rotate(model_angle*0.5f, 1.0f, 0.0f, 0.0f) * mat_Model;

  mat_PVM = mat_Proj*mat_View_inv*mat_Model;

  kmuvcl::math::mat3x3f mat_Normal;

  mat_Normal(0, 0) = mat_Model(0, 0);
  mat_Normal(0, 1) = mat_Model(0, 1);
  mat_Normal(0, 2) = mat_Model(0, 2);
  mat_Normal(1, 0) = mat_Model(1, 0);
  mat_Normal(1, 1) = mat_Model(1, 1);
  mat_Normal(1, 2) = mat_Model(1, 2);
  mat_Normal(2, 0) = mat_Model(2, 0);
  mat_Normal(2, 1) = mat_Model(2, 1);
  mat_Normal(2, 2) = mat_Model(2, 2);

  kmuvcl::math::mat4x4f mat_View = kmuvcl::math::inverse(mat_View_inv);

	glUniformMatrix4fv(loc_u_pvm_matrix, 1, false, mat_PVM);
  glUniformMatrix4fv(loc_u_model_matrix, 1, false, mat_Model);
  glUniformMatrix4fv(loc_u_view_matrix, 1, false, mat_View);
  glUniformMatrix3fv(loc_u_normal_matrix, 1, false, mat_Normal);

  glUniform3fv(loc_u_light_vector, 1, light_vector);
  glUniform4fv(loc_u_light_ambient, 1, light_ambient);
  glUniform4fv(loc_u_light_diffuse, 1, light_diffuse);
  glUniform4fv(loc_u_light_specular, 1, light_specular);
  Shader::check_gl_error("glUniform4fv");

  g_model.draw(loc_a_vertex, loc_a_normal,
    loc_u_material_ambient, loc_u_material_diffuse,
    loc_u_material_specular, loc_u_material_shininess);
  Shader::check_gl_error("draw");

	glUseProgram(0);

  glutSwapBuffers();
}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}

void keyboard(unsigned char key, int x, int y)
{

    if(key == 'a' || key == 'A')
  {
    g_camera.rotate_left(5.0f);
    glutPostRedisplay(); //os한테 다시 그려달라고 요청하는 것!  mydisplay를 호출하는게 아니다.
  }
  else if(key == 'd' || key == 'D')
  {
    g_camera.rotate_right(5.0f);
    glutPostRedisplay(); //os한테 다시 그려달라고 요청하는 것!  mydisplay를 호출하는게 아니다.
  }

  else if(key == 'w' || key == 'W') // 상승
  {
    float delta = 0.1f;
    g_camera.fly(delta);
    glutPostRedisplay();
  }
  else if(key == 's' || key == 'S') // 하강
  {
    float delta = -0.1f;
    g_camera.fly(delta);
    glutPostRedisplay();
  }
}

void special(int key, int x, int y)
{
	// TODO: properly handle special keyboard event
  switch(key)
      {
      case GLUT_KEY_UP:
          g_camera.move_forward(0.5f);
          break;
      case GLUT_KEY_DOWN:
          g_camera.move_backward(0.5f);
          break;
      case GLUT_KEY_LEFT:
          g_camera.move_left(0.5f);
          break;
      case GLUT_KEY_RIGHT:
          g_camera.move_right(0.5f);
          break;
      }

      glutPostRedisplay();
}

void idle()
{
  curr = std::chrono::system_clock::now();

  std::chrono::duration<float> elaped_seconds = (curr - prev);

  model_angle += 10 * elaped_seconds.count();
  prev = curr;

  glutPostRedisplay();
}
