// Truong Ngoc Trung Anh - 2020004

#include <math.h>
#include <iostream>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <cmath>
#define PI 3.14159265358979323846

using namespace std;

class PointXYZ
{
public:
	float x, y, z;
	void set(float dx, float dy, float dz)
	{
		x = dx;
		y = dy;
		z = dz;
	}
	void set(PointXYZ &ptXYZ)
	{
		x = ptXYZ.x;
		y = ptXYZ.y;
		z = ptXYZ.z;
	}
	PointXYZ()
	{
		x = 0;
		y = 0;
		z = 0;
	}
	PointXYZ(float dx, float dy, float dz)
	{
		x = dx;
		y = dy;
		z = dz;
	}
};

class VectorXYZ
{
public:
	float x, y, z;
	void set(float dx, float dy, float dz)
	{
		x = dx;
		y = dy;
		z = dz;
	}
	void set(VectorXYZ &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}
	void flip()
	{
		x = -x;
		y = -y;
		z = -z;
	}
	void normalize();
	VectorXYZ() { x = y = z = 0; }
	VectorXYZ(float dx, float dy, float dz)
	{
		x = dx;
		y = dy;
		z = dz;
	}
	VectorXYZ(VectorXYZ &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}
	VectorXYZ cross(VectorXYZ b);
	float dot(VectorXYZ b);
};

VectorXYZ VectorXYZ::cross(VectorXYZ b)
{
	VectorXYZ c(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x);
	return c;
}
float VectorXYZ::dot(VectorXYZ b)
{
	return x * b.x + y * b.y + z * b.z;
}
void VectorXYZ::normalize()
{
	float tmp = sqrt(x * x + y * y + z * z);
	x = x / tmp;
	y = y / tmp;
	z = z / tmp;
}

class Vertex
{
public:
	int vertexIndex;
	int colorIndex;
};

class Face
{
public:
	int nVerts;
	Vertex *vert;
	VectorXYZ facenorm;

	Face()
	{
		nVerts = 0;
		vert = NULL;
	}
	~Face()
	{
		if (vert != NULL)
		{
			delete[] vert;
			vert = NULL;
		}
		nVerts = 0;
	}
};

class Mesh
{
public:
	int numVerts;
	PointXYZ *pt;

	int numFaces;
	Face *face;

	float slideX, slideY, slideZ;
	float rotateX, rotateY, rotateZ;
	float scaleX, scaleY, scaleZ;

public:
	Mesh()
	{
		numVerts = 0;
		pt = NULL;
		numFaces = 0;
		face = NULL;
		slideX = 0;
		slideY = 0;
		slideZ = 0;
		scaleX = 0;
		scaleY = 0;
		scaleZ = 0;
		rotateX = 0;
		rotateY = 0;
		rotateZ = 0;
	}
	~Mesh()
	{
		if (pt != NULL)
		{
			delete[] pt;
		}
		if (face != NULL)
		{
			delete[] face;
		}
		numVerts = 0;
		numFaces = 0;
	}
	void DrawWireframe();
	void SetColor(int colorIdx);
	void veCanhQuat(int N, float rong, float cao, float banKinhNho);
	void veGoiDo(int N, float chieuCao, float BKtrong, float BKngoai);
	void veLK2lo1(int N, float doDay, float KhoangCachTam, float BKtrong, float BKngoai);
	void veHinhTru(int N, float base1Height, float base1Radius);
	void CalculateFacesNorm();
	void Draw();
	void setupMaterial(float ambient[], float diffuse[], float specular[], float shiness);
};

void Mesh::DrawWireframe()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for (int f = 0; f < numFaces; f++)
	{
		glBegin(GL_POLYGON);
		for (int v = 0; v < face[f].nVerts; v++)
		{
			int iv = face[f].vert[v].vertexIndex;
			glVertex3f(pt[iv].x, pt[iv].y, pt[iv].z);
		}
		glEnd();
	}
}

void Mesh::CalculateFacesNorm()
{
	for (int f = 0; f < numFaces; f++)
	{
		float mx = 0, my = 0, mz = 0;
		for (int v = 0; v < face[f].nVerts; v++)
		{
			int iv = face[f].vert[v].vertexIndex;
			int next = face[f].vert[(v + 1) % face[f].nVerts].vertexIndex;
			mx += (pt[iv].y - pt[next].y) * (pt[iv].z + pt[next].z);
			my += (pt[iv].z - pt[next].z) * (pt[iv].x + pt[next].x);
			mz += (pt[iv].x - pt[next].x) * (pt[iv].y + pt[next].y);
		}
		face[f].facenorm.set(mx, my, mz);
		face[f].facenorm.normalize();
	}
}

void Mesh::Draw()
{
	for (int f = 0; f < numFaces; f++)
	{
		glBegin(GL_POLYGON);
		for (int v = 0; v < face[f].nVerts; v++)
		{
			int iv = face[f].vert[v].vertexIndex;
			glNormal3f(face[f].facenorm.x, face[f].facenorm.y, face[f].facenorm.z);
			glVertex3f(pt[iv].x, pt[iv].y, pt[iv].z);
		}
		glEnd();
	}
}

void Mesh::setupMaterial(float ambient[], float diffuse[], float specular[], float shiness)
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiness);
}
// Hinh hop chu nhat
void Mesh::veHinhTru(int N, float chieuCao, float banKinh)
{
	int i;
	numVerts = 2 * N + 2;
	GLfloat angle = 2 * PI / N;
	pt = new PointXYZ[numVerts];
	for (i = 0; i < N; i++)
	{
		pt[i].set(banKinh * sin((double)i * angle), -banKinh * cos((double)i * angle), chieuCao / 2);
	}
	for (i = 0; i < N; i++)
	{
		pt[i + N].set(banKinh * sin((double)i * angle), -banKinh * cos((double)i * angle), -chieuCao / 2);
	}

	pt[2 * N].set(0, 0, chieuCao / 2);
	pt[2 * N + 1].set(0, 0, -chieuCao / 2);

	numFaces = 3 * N;
	face = new Face[numFaces];

	for (i = 0; i < numFaces; i++)
	{
		if (i < N - 1)
		{
			face[i].nVerts = 3;
			face[i].vert = new Vertex[face[i].nVerts];

			face[i].vert[0].vertexIndex = i;
			face[i].vert[1].vertexIndex = i + 1;
			face[i].vert[2].vertexIndex = 2 * N;
		}
		else if (i == N - 1)
		{
			face[i].nVerts = 3;
			face[i].vert = new Vertex[face[i].nVerts];

			face[i].vert[0].vertexIndex = i;
			face[i].vert[1].vertexIndex = 0;
			face[i].vert[2].vertexIndex = 2 * N;
		}
		else if (i < 2 * N - 1)
		{
			face[i].nVerts = 3;
			face[i].vert = new Vertex[face[i].nVerts];

			face[i].vert[0].vertexIndex = i;
			face[i].vert[1].vertexIndex = 2 * N + 1;
			face[i].vert[2].vertexIndex = i + 1;
		}
		else if (i == 2 * N - 1)
		{
			face[i].nVerts = 3;
			face[i].vert = new Vertex[face[i].nVerts];

			face[i].vert[0].vertexIndex = i;
			face[i].vert[1].vertexIndex = 2 * N + 1;
			face[i].vert[2].vertexIndex = N;
		}
		else if (i < 3 * N - 1)
		{
			face[i].nVerts = 4;
			face[i].vert = new Vertex[face[i].nVerts];

			face[i].vert[0].vertexIndex = i - 2 * N + 1;
			face[i].vert[1].vertexIndex = i - 2 * N;
			face[i].vert[2].vertexIndex = i - N;
			face[i].vert[3].vertexIndex = i - N + 1;
		}
		else
		{
			face[i].nVerts = 4;
			face[i].vert = new Vertex[face[i].nVerts];

			face[i].vert[0].vertexIndex = 0;
			face[i].vert[1].vertexIndex = N - 1;
			face[i].vert[2].vertexIndex = 2 * N - 1;
			face[i].vert[3].vertexIndex = N;
		}
		for (int j = 0; j < face[i].nVerts; j++)
			face[i].vert[j].colorIndex = i;
	}
}
// Vẽ cánh quạt
void Mesh::veCanhQuat(int N, float rong, float cao, float banKinhNho)
{
	float KhoangCachTamToiDinhCuoi = tan(30 * PI / 180) * (banKinhNho / cos(10 * PI / 180) + cao / tan(80 * PI / 180)) + cao;
	int M = (N - 1) * 2; // M: So diem tren mot hinh tron
	GLfloat angle = 2 * PI / M;

	int i = 0;
	numVerts = M * 2 + N * 2 + N * 2 + 4;
	pt = new PointXYZ[numVerts];
	// Vong tron  ben trai: [0, M - 1]
	for (int j = 0; j < M; j++)
	{
		pt[i].set(banKinhNho * cos((double)j * angle), KhoangCachTamToiDinhCuoi + banKinhNho * sin((double)j * angle), rong / 2);
		i++;
	}
	// Vong tron  ben phai: [M, 2M - 1]
	for (int j = 0; j < M; j++)
	{
		pt[i].set(banKinhNho * cos((double)j * angle), KhoangCachTamToiDinhCuoi + banKinhNho * sin((double)j * angle), -rong / 2);
		i++;
	}
	// 4 đỉnh ở đầu
	// 2M
	pt[i].set(banKinhNho * cos(10 * PI / 180), banKinhNho * sin(10 * PI / 180) + KhoangCachTamToiDinhCuoi, rong / 2);
	i++;
	// 2M + 1
	pt[i].set(-banKinhNho * cos(10 * PI / 180), banKinhNho * sin(10 * PI / 180) + KhoangCachTamToiDinhCuoi, rong / 2);
	i++;
	// 2M + 2
	pt[i].set(banKinhNho * cos(10 * PI / 180), banKinhNho * sin(10 * PI / 180) + KhoangCachTamToiDinhCuoi, -rong / 2);
	i++;
	// 2M + 3
	pt[i].set(-banKinhNho * cos(10 * PI / 180), banKinhNho * sin(10 * PI / 180) + KhoangCachTamToiDinhCuoi, -rong / 2);
	i++;
	//  4 đỉnh của 4 góc ở đít
	// 2M + 4
	float kc1 = tan(30 * PI / 180) * (banKinhNho / cos(10 * PI / 180) + cao / tan(80 * PI / 180));
	pt[i].set(banKinhNho / cos(10 * PI / 180) + cao / tan(80 * PI / 180), kc1, rong / 2);
	i++;
	// 2M + 5
	pt[i].set(-banKinhNho / cos(10 * PI / 180) - cao / tan(80 * PI / 180), kc1, rong / 2);
	i++;
	// 2M + 6
	pt[i].set(banKinhNho / cos(10 * PI / 180) + cao / tan(80 * PI / 180), kc1, -rong / 2);
	i++;
	// 2M + 7
	pt[i].set(-banKinhNho / cos(10 * PI / 180) - cao / tan(80 * PI / 180), kc1, -rong / 2);
	i++;
	// Tâm trái : 2M + 8
	pt[i].set(0, KhoangCachTamToiDinhCuoi, rong / 2);
	i++;
	// Tâm phải : 2M + 9
	pt[i].set(0, KhoangCachTamToiDinhCuoi, -rong / 2);
	i++;
	//Đỉnh cuối trái : 2M + 10
	pt[i].set(0, 0, rong / 2);
	i++;
	//Đỉnh cuối phải : 2M + 11
	pt[i].set(0, 0, -rong / 2);
	i++;
	/******************************************************************************************************************/
	numFaces = 10 + M - 1 + M - 1 + M - 1;
	face = new Face[numFaces];
	int mau = 0;
	i = 0;
	// To màu Chiều dày khu hình tròn
	for (int j = 0; j < (M - 1); j++)
	{
		face[i].nVerts = 4;
		face[i].vert = new Vertex[face[i].nVerts];

		face[i].vert[0].vertexIndex = j;
		face[i].vert[1].vertexIndex = j + 1;
		face[i].vert[2].vertexIndex = j + M + 1;
		face[i].vert[3].vertexIndex = j + M;

		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = mau;
		}
		mau++;
		i++;
	}
	// To màu mặt trái hình tròn
	for (int j = 0; j < (M - 1); j++)
	{
		face[i].nVerts = 4;
		face[i].vert = new Vertex[face[i].nVerts];

		face[i].vert[0].vertexIndex = j;
		face[i].vert[1].vertexIndex = j + 1;
		face[i].vert[2].vertexIndex = 2 * M + 8;
		face[i].vert[3].vertexIndex = 2 * M + 8;

		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = mau;
		}
		mau++;
		i++;
	}
	// To màu mặt phải hình tròn
	for (int j = 0; j < (M - 1); j++)
	{
		face[i].nVerts = 4;
		face[i].vert = new Vertex[face[i].nVerts];

		face[i].vert[0].vertexIndex = j + M;
		face[i].vert[1].vertexIndex = j + M + 1;
		face[i].vert[2].vertexIndex = 2 * M + 9;
		face[i].vert[3].vertexIndex = 2 * M + 9;

		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = mau;
		}
		mau++;
		i++;
	}

	// To màu mặt trái hình thang (1/6)
	face[i].nVerts = 4;
	face[i].vert = new Vertex[face[i].nVerts];

	face[i].vert[0].vertexIndex = 2 * M;
	face[i].vert[1].vertexIndex = 2 * M + 1;
	face[i].vert[2].vertexIndex = 2 * M + 5;
	face[i].vert[3].vertexIndex = 2 * M + 4;
	for (int k = 0; k < face[i].nVerts; k++)
	{
		face[i].vert[k].colorIndex = mau;
	}
	mau++;
	i++;
	// Tô màu mặt trái hình thang (2/6)
	face[i].nVerts = 4;
	face[i].vert = new Vertex[face[i].nVerts];

	face[i].vert[0].vertexIndex = 2 * M + 2;
	face[i].vert[1].vertexIndex = 2 * M + 3;
	face[i].vert[2].vertexIndex = 2 * M + 7;
	face[i].vert[3].vertexIndex = 2 * M + 6;
	for (int k = 0; k < face[i].nVerts; k++)
	{
		face[i].vert[k].colorIndex = mau;
	}
	mau++;
	i++;
	// Tô màu mặt chiều dày đáy hình thang (3/6)
	face[i].nVerts = 4;
	face[i].vert = new Vertex[face[i].nVerts];

	face[i].vert[0].vertexIndex = 2 * M + 7;
	face[i].vert[1].vertexIndex = 2 * M + 6;
	face[i].vert[2].vertexIndex = 2 * M + 4;
	face[i].vert[3].vertexIndex = 2 * M + 5;
	for (int k = 0; k < face[i].nVerts; k++)
	{
		face[i].vert[k].colorIndex = mau;
	}
	mau++;
	i++;
	// Tô màu mặt chiều dày đỉnh hình thang (4/6)
	face[i].nVerts = 4;
	face[i].vert = new Vertex[face[i].nVerts];

	face[i].vert[0].vertexIndex = 2 * M + 0;
	face[i].vert[1].vertexIndex = 2 * M + 1;
	face[i].vert[2].vertexIndex = 2 * M + 3;
	face[i].vert[3].vertexIndex = 2 * M + 2;
	for (int k = 0; k < face[i].nVerts; k++)
	{
		face[i].vert[k].colorIndex = mau;
	}
	mau++;
	i++;
	// Tô màu mặt chiều dày mặt bên x > 0  đỉnh hình thang (5/6)
	face[i].nVerts = 4;
	face[i].vert = new Vertex[face[i].nVerts];

	face[i].vert[0].vertexIndex = 2 * M + 0;
	face[i].vert[1].vertexIndex = 2 * M + 2;
	face[i].vert[2].vertexIndex = 2 * M + 6;
	face[i].vert[3].vertexIndex = 2 * M + 4;
	for (int k = 0; k < face[i].nVerts; k++)
	{
		face[i].vert[k].colorIndex = mau;
	}
	mau++;
	i++;
	// Tô màu mặt chiều dày mặt bên x < 0  đỉnh hình thang (6/6)
	face[i].nVerts = 4;
	face[i].vert = new Vertex[face[i].nVerts];

	face[i].vert[0].vertexIndex = 2 * M + 1;
	face[i].vert[1].vertexIndex = 2 * M + 3;
	face[i].vert[2].vertexIndex = 2 * M + 7;
	face[i].vert[3].vertexIndex = 2 * M + 5;
	for (int k = 0; k < face[i].nVerts; k++)
	{
		face[i].vert[k].colorIndex = mau;
	}
	mau++;
	i++;
	// Tô màu mặt trái tam giác.
	face[i].nVerts = 3;
	face[i].vert = new Vertex[face[i].nVerts];

	face[i].vert[0].vertexIndex = 2 * M + 5;
	face[i].vert[1].vertexIndex = 2 * M + 4;
	face[i].vert[2].vertexIndex = 2 * M + 10;
	// face[i].vert[3].vertexIndex = 2 * M + 5;
	for (int k = 0; k < face[i].nVerts; k++)
	{
		face[i].vert[k].colorIndex = mau;
	}
	mau++;
	i++;
	// Tô màu mặt phải tam giác.
	face[i].nVerts = 3;
	face[i].vert = new Vertex[face[i].nVerts];

	face[i].vert[0].vertexIndex = 2 * M + 6;
	face[i].vert[1].vertexIndex = 2 * M + 7;
	face[i].vert[2].vertexIndex = 2 * M + 11;
	// face[i].vert[3].vertexIndex = 2 * M + 5;
	for (int k = 0; k < face[i].nVerts; k++)
	{
		face[i].vert[k].colorIndex = mau;
	}
	mau++;
	i++;
	// Tô màu mặt bên tam giác x < 0.
	face[i].nVerts = 4;
	face[i].vert = new Vertex[face[i].nVerts];

	face[i].vert[0].vertexIndex = 2 * M + 5;
	face[i].vert[1].vertexIndex = 2 * M + 10;
	face[i].vert[2].vertexIndex = 2 * M + 11;
	face[i].vert[3].vertexIndex = 2 * M + 7;
	for (int k = 0; k < face[i].nVerts; k++)
	{
		face[i].vert[k].colorIndex = mau;
	}
	mau++;
	i++;
	// Tô màu mặt bên tam giác x > 0.
	face[i].nVerts = 4;
	face[i].vert = new Vertex[face[i].nVerts];

	face[i].vert[0].vertexIndex = 2 * M + 4;
	face[i].vert[1].vertexIndex = 2 * M + 10;
	face[i].vert[2].vertexIndex = 2 * M + 11;
	face[i].vert[3].vertexIndex = 2 * M + 6;
	for (int k = 0; k < face[i].nVerts; k++)
	{
		face[i].vert[k].colorIndex = mau;
	}
	mau++;
	i++;
	// Tô màu mặt đáy tam giác.
	face[i].nVerts = 4;
	face[i].vert = new Vertex[face[i].nVerts];

	face[i].vert[0].vertexIndex = 2 * M + 4;
	face[i].vert[1].vertexIndex = 2 * M + 5;
	face[i].vert[2].vertexIndex = 2 * M + 6;
	face[i].vert[3].vertexIndex = 2 * M + 7;
	for (int k = 0; k < face[i].nVerts; k++)
	{
		face[i].vert[k].colorIndex = mau;
	}
	mau++;
	i++;
}
void Mesh::veGoiDo(int N, float chieuCao, float BKtrong, float BKngoai)
{
	int i = 0;
	N = 4;
	numVerts = 4 * N + 2;
	GLfloat angle = PI / 4 / (N - 1); // 45 độ
	pt = new PointXYZ[numVerts];
	float L = cos(135 / 2 * PI / 180) * BKngoai * 2;
	//[0,N-1]
	for (int j = 0; j < N; j++)
	{
		pt[i].set(sin(135 / 2 * PI / 180) * L / (N - 1) * j, BKngoai - cos(135 / 2 * PI / 180) * L / (N - 1) * j, chieuCao / 2);
		i++;
	}
	//[N,2N-1]
	for (int j = 0; j < N; j++)
	{
		pt[i].set(sin(135 / 2 * PI / 180) * L / (N - 1) * j, BKngoai - cos(135 / 2 * PI / 180) * L / (N - 1) * j, -chieuCao / 2);
		i++;
	}
	//[2N,3N-1]
	for (int j = 0; j < N; j++)
	{
		pt[i].set(BKtrong * sin((double)j * angle), BKtrong * cos((double)j * angle), chieuCao / 2);
		i++;
	}
	//[3N,4N-1]
	for (int j = 0; j < N; j++)
	{
		pt[i].set(BKtrong * sin((double)j * angle), BKtrong * cos((double)j * angle), -chieuCao / 2);
		i++;
	}
	// 4N
	pt[i].set(0, 0, chieuCao / 2);
	i++;

	numFaces = 4 * N;
	face = new Face[numFaces];
	int mau = 0;
	i = 0;
	// măt trái
	for (int j = 0; j < N - 1; j++)
	{
		face[i].nVerts = 4;
		face[i].vert = new Vertex[face[i].nVerts];

		face[i].vert[0].vertexIndex = j;
		face[i].vert[1].vertexIndex = j + 1;
		face[i].vert[2].vertexIndex = 2 * N + 1 + j;
		face[i].vert[3].vertexIndex = 2 * N + j;

		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = mau;
		}
		mau++;
		i++;
	}
	// mặt phải
	for (int j = 0; j < N - 1; j++)
	{
		face[i].nVerts = 4;
		face[i].vert = new Vertex[face[i].nVerts];

		face[i].vert[0].vertexIndex = N + j;
		face[i].vert[1].vertexIndex = N + j + 1;
		face[i].vert[2].vertexIndex = 3 * N + 1 + j;
		face[i].vert[3].vertexIndex = 3 * N + j;

		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = mau;
		}
		mau++;
		i++;
	}
	// mặt bỏ (độ dày)
	for (int j = 0; j < N - 1; j++)
	{
		face[i].nVerts = 4;
		face[i].vert = new Vertex[face[i].nVerts];

		face[i].vert[0].vertexIndex = j;
		face[i].vert[1].vertexIndex = j + 1;
		face[i].vert[2].vertexIndex = N + 1 + j;
		face[i].vert[3].vertexIndex = N + j;

		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = mau;
		}
		mau++;
		i++;
	}
	// mặt bỏ (độ dày)
	for (int j = 0; j < N - 1; j++)
	{
		face[i].nVerts = 4;
		face[i].vert = new Vertex[face[i].nVerts];

		face[i].vert[0].vertexIndex = 2 * N + j;
		face[i].vert[1].vertexIndex = 2 * N + j + 1;
		face[i].vert[2].vertexIndex = 3 * N + 1 + j;
		face[i].vert[3].vertexIndex = 3 * N + j;

		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = mau;
		}
		mau++;
		i++;
	}
}
void Mesh::veLK2lo1(int N, float doDay, float KhoangCachTam, float BKtrong, float BKngoai)
{
	int M = (N - 1) * 2; // M: So diem tren mot hinh tron
	GLfloat angle = 2 * PI / (M - 1);
	int i = 0;
	numVerts = M * 9;
	pt = new PointXYZ[numVerts];

	// vòng tròn ngoài phía trên ben trai: [0, M - 1]
	for (int j = 0; j < M; j++)
	{
		pt[i].set(BKngoai * cos((double)j * angle), KhoangCachTam / 2 + BKngoai * sin((double)j * angle), doDay / 2);
		i++;
	}
	// vòng tròn ngoài phía trên ben phai: [M, 2M - 1]
	for (int j = 0; j < M; j++)
	{
		pt[i].set(BKngoai * cos((double)j * angle), KhoangCachTam / 2 + BKngoai * sin((double)j * angle), -doDay / 2);
		i++;
	}
	// Vong tron trong phía trên ben trai: [2M, 3M - 1]
	for (int j = 0; j < M; j++)
	{
		pt[i].set(BKtrong * cos((double)j * angle), KhoangCachTam / 2 + BKtrong * sin((double)j * angle), doDay / 2);
		i++;
	}
	// Vong tron trong phía trên ben phai: [3M, 4M - 1]
	for (int j = 0; j < M; j++)
	{
		pt[i].set(BKtrong * cos((double)j * angle), KhoangCachTam / 2 + BKtrong * sin((double)j * angle), -doDay / 2);
		i++;
	}
	///////////////////////////////////////////////
	// // vòng tròn ngoài phía dưới ben trai: [4M, 5M - 1]
	for (int j = 0; j < M; j++)
	{
		pt[i].set(BKngoai * cos((double)j * angle), -KhoangCachTam / 2 - BKngoai * sin((double)j * angle), doDay / 2);
		i++;
	}
	// vòng tròn ngoài phía dưới ben phai: [5M, 6M - 1]
	for (int j = 0; j < M; j++)
	{
		pt[i].set(BKngoai * cos((double)j * angle), -KhoangCachTam / 2 - BKngoai * sin((double)j * angle), -doDay / 2);
		i++;
	}
	// Vong tron trong phía dưới ben trai: [6M, 7M - 1]
	for (int j = 0; j < M; j++)
	{
		pt[i].set(BKtrong * cos((double)j * angle), -KhoangCachTam / 2 - BKtrong * sin((double)j * angle), doDay / 2);
		i++;
	}
	// Vong tron trong phía dưới ben phai: [7M, 8M - 1]
	for (int j = 0; j < M; j++)
	{
		pt[i].set(BKtrong * cos((double)j * angle), -KhoangCachTam / 2 - BKtrong * sin((double)j * angle), -doDay / 2);
		i++;
	}

	// tâm lỗ trên bên trái 8M
	pt[i].set(0, KhoangCachTam / 2, doDay / 2);
	i++;
	// tâm lỗ trên bên phải 8M+1
	pt[i].set(0, KhoangCachTam / 2, -doDay / 2);
	i++;
	// tâm lỗ dưới bên trái 8M
	pt[i].set(0, -KhoangCachTam / 2, doDay / 2);
	i++;
	// tâm lỗ dưới bên phải 8M+1
	pt[i].set(0, -KhoangCachTam / 2, -doDay / 2);
	i++;

	numFaces = 10 * M + 4;
	face = new Face[numFaces];
	int mau = 0;
	i = 0;
	// măt trái trên
	for (int j = 0; j < M - 1; j++)
	{
		face[i].nVerts = 4;
		face[i].vert = new Vertex[face[i].nVerts];

		face[i].vert[0].vertexIndex = j;
		face[i].vert[1].vertexIndex = j + 1;
		face[i].vert[2].vertexIndex = 2 * M + 1 + j;
		face[i].vert[3].vertexIndex = 2 * M + j;

		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = mau;
		}
		mau++;
		i++;
	}
	// măt phải trên
	for (int j = 0; j < M - 1; j++)
	{
		face[i].nVerts = 4;
		face[i].vert = new Vertex[face[i].nVerts];

		face[i].vert[0].vertexIndex = M + j;
		face[i].vert[1].vertexIndex = M + j + 1;
		face[i].vert[2].vertexIndex = 3 * M + 1 + j;
		face[i].vert[3].vertexIndex = 3 * M + j;

		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = mau;
		}
		mau++;
		i++;
	}
	// mặt trong trên
	for (int j = 0; j < M - 1; j++)
	{
		face[i].nVerts = 4;
		face[i].vert = new Vertex[face[i].nVerts];

		face[i].vert[0].vertexIndex = 2 * M + j;
		face[i].vert[1].vertexIndex = 2 * M + j + 1;
		face[i].vert[2].vertexIndex = 3 * M + 1 + j;
		face[i].vert[3].vertexIndex = 3 * M + j;

		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = mau;
		}
		mau++;
		i++;
	}
	// mặt ngoài trên
	for (int j = 0; j < M - 1; j++)
	{
		face[i].nVerts = 4;
		face[i].vert = new Vertex[face[i].nVerts];

		face[i].vert[0].vertexIndex = j;
		face[i].vert[1].vertexIndex = j + 1;
		face[i].vert[2].vertexIndex = M + 1 + j;
		face[i].vert[3].vertexIndex = M + j;

		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = mau;
		}
		mau++;
		i++;
	}
	///// tròn dưới
	// măt trái trên
	for (int j = 0; j < M - 1; j++)
	{
		face[i].nVerts = 4;
		face[i].vert = new Vertex[face[i].nVerts];

		face[i].vert[0].vertexIndex = 4 * M + j;
		face[i].vert[1].vertexIndex = 4 * M + j + 1;
		face[i].vert[2].vertexIndex = 6 * M + 1 + j;
		face[i].vert[3].vertexIndex = 6 * M + j;

		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = mau;
		}
		mau++;
		i++;
	}
	// măt phải trên
	for (int j = 0; j < M - 1; j++)
	{
		face[i].nVerts = 4;
		face[i].vert = new Vertex[face[i].nVerts];

		face[i].vert[0].vertexIndex = 5 * M + j;
		face[i].vert[1].vertexIndex = 5 * M + j + 1;
		face[i].vert[2].vertexIndex = 7 * M + 1 + j;
		face[i].vert[3].vertexIndex = 7 * M + j;

		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = mau;
		}
		mau++;
		i++;
	}

	// mặt trong lỗ dưói
	for (int j = 0; j < M - 1; j++)
	{
		face[i].nVerts = 4;
		face[i].vert = new Vertex[face[i].nVerts];

		face[i].vert[0].vertexIndex = 6 * M + j;
		face[i].vert[1].vertexIndex = 6 * M + j + 1;
		face[i].vert[2].vertexIndex = 7 * M + 1 + j;
		face[i].vert[3].vertexIndex = 7 * M + j;

		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = mau;
		}
		mau++;
		i++;
	}
	// mặt ngoài lỗ dưới
	for (int j = 0; j < M - 1; j++)
	{
		face[i].nVerts = 4;
		face[i].vert = new Vertex[face[i].nVerts];

		face[i].vert[0].vertexIndex = 4 * M + j;
		face[i].vert[1].vertexIndex = 4 * M + j + 1;
		face[i].vert[2].vertexIndex = 5 * M + 1 + j;
		face[i].vert[3].vertexIndex = 5 * M + j;

		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = mau;
		}
		mau++;
		i++;
	}

	// mặt bên
	face[i].nVerts = 4;
	face[i].vert = new Vertex[face[i].nVerts];

	face[i].vert[0].vertexIndex = 0;
	face[i].vert[1].vertexIndex = M;
	face[i].vert[2].vertexIndex = 5 * M;
	face[i].vert[3].vertexIndex = 4 * M;

	for (int k = 0; k < face[i].nVerts; k++)
	{
		face[i].vert[k].colorIndex = mau;
	}
	mau++;
	i++;

	face[i].nVerts = 4;
	face[i].vert = new Vertex[face[i].nVerts];

	face[i].vert[0].vertexIndex = (M - 1) / 2;
	face[i].vert[1].vertexIndex = M + (M - 1) / 2;
	face[i].vert[2].vertexIndex = 5 * M + (M - 1) / 2;
	face[i].vert[3].vertexIndex = 4 * M + (M - 1) / 2;

	for (int k = 0; k < face[i].nVerts; k++)
	{
		face[i].vert[k].colorIndex = mau;
	}
	mau++;
	i++;

	// mặt giữa trái
	for (int j = 0; j < M / 2 - 1; j++)
	{
		face[i].nVerts = 4;
		face[i].vert = new Vertex[face[i].nVerts];

		face[i].vert[0].vertexIndex = M / 2 + j;
		face[i].vert[1].vertexIndex = M / 2 + j + 1;
		face[i].vert[2].vertexIndex = 4 * M + M / 2 + 1 + j;
		face[i].vert[3].vertexIndex = 4 * M + M / 2 + j;

		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = mau;
		}
		mau++;
		i++;
	}
	// mặt giữa phải
	for (int j = 0; j < M / 2 - 1; j++)
	{
		face[i].nVerts = 4;
		face[i].vert = new Vertex[face[i].nVerts];

		face[i].vert[0].vertexIndex = M + M / 2 + j;
		face[i].vert[1].vertexIndex = M + M / 2 + j + 1;
		face[i].vert[2].vertexIndex = 5 * M + M / 2 + 1 + j;
		face[i].vert[3].vertexIndex = 5 * M + M / 2 + j;

		for (int k = 0; k < face[i].nVerts; k++)
		{
			face[i].vert[k].colorIndex = mau;
		}
		mau++;
		i++;
	}
}

// thiết lập tham số

// cửa sổ màn hình
int screenWidth = 1200;
int screenHeight = 600;
// Tham so cua vat the
float base1Radius = 0.8;
float base1Height = 0.2;
float base1RotateStep = 1;
// banxoay
float banxoayCao = 0.18;
float banxoayBanKinh = 0.5;
float gocXoay = 4;
// tayquay
float tayquayDai = 0.9 * banxoayBanKinh;
float tayquayCao = banxoayCao * 0.8;
// canhQuat1_1
float banKinhBoTronCanhQuat = 0.2;
// bán kính bo tròn
float caoCanhQuat = 1.5;
float dayCanhQuat = 0.25;
float KCtoiTam = tan(30 * PI / 180) * (banKinhBoTronCanhQuat / cos(10 * PI / 180) + caoCanhQuat / tan(80 * PI / 180)) + caoCanhQuat;
float KC1 = tan(30 * PI / 180) * (banKinhBoTronCanhQuat / cos(10 * PI / 180) + caoCanhQuat / tan(80 * PI / 180));

float canhQuat1_1_chieuDay = dayCanhQuat;
float canhQuat1_1Cao = caoCanhQuat;
float canhQuat1_1CaoBanKinhLon = 1.5 * 0.8 / 6;
float canhQuat1_1CaoBanKinhNho = banKinhBoTronCanhQuat;
// canhQuat1_2
float canhQuat1_2Rong = dayCanhQuat;
float canhQuat1_2Cao = caoCanhQuat;
float canhQuat1_2CaoBanKinhLon = 1.5 * 0.8 / 6;
float canhQuat1_2CaoBanKinhNho = banKinhBoTronCanhQuat;

float goiDoDay = 0.3;
float trucToDai = 0.8;
float trucToBanKinh = 0.15;

float trucBatGiacDai = 0.8;
float trucBatGiacBanKinh = 0.1;

float doDaiLK = 0.2 / (cos(atan(1.3 / 0.2))); // khoảng cách giữa 2 tâm
Mesh banxoay;
Mesh canhQuat1_1;
Mesh canhQuat1_2;
Mesh canhQuat1_3;
Mesh LK2lo1;
Mesh LK2lo2;
Mesh goiDo;
Mesh trucBatGiac;
Mesh chot;
Mesh chotMauHong;
//
double angle = 0;
bool au = false;
bool cheDoLuoi = false;
bool doiCheDoNhin = false;
bool bLight1 = true;
// khởi tạo tham số camera
float gocNhin;
float gocNhinLenXuong;
float khoangCachNhin;
float gocNhinX, gocNhinY, gocNhinZ;
float nhinTrucX, nhinTrucY, nhinTrucZ;

void pressKeyBoard2(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		gocNhinLenXuong += 0.5;
		break;
	case GLUT_KEY_DOWN:
		gocNhinLenXuong -= 0.5;
		if (gocNhinLenXuong < 0)
			gocNhinLenXuong = 0;
		break;
	case GLUT_KEY_RIGHT:
		// -> nguoc kim dong ho
		gocNhin -= 3;
		break;
	case GLUT_KEY_LEFT:
		// -> cung kim dong ho
		gocNhin += 3;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}
void pressKeyBoard1(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '1':
		banxoay.rotateY += gocXoay;
		if (banxoay.rotateY > 360)
			banxoay.rotateY -= 360;
		break;
	case '2':
		banxoay.rotateY -= gocXoay;
		if (banxoay.rotateY < 0)
			banxoay.rotateY += 360;
		break;
	case 'w':
	case 'W':
		cheDoLuoi = !cheDoLuoi;
		break;
	case 'v':
	case 'V':
		if (doiCheDoNhin)
		{
			doiCheDoNhin = !doiCheDoNhin;
			gocNhin = 45;
			gocNhinLenXuong = 6;
			khoangCachNhin = 6.5;
		}
		else
		{
			doiCheDoNhin = !doiCheDoNhin;
			gocNhinLenXuong = 20;
			khoangCachNhin = 0;
			gocNhin = 180;
		}
		break;
	case '+':
		khoangCachNhin += 0.5;
		break;
	case '-':
		khoangCachNhin -= 0.5;
		break;
	}
	glutPostRedisplay();
}

// vẽ nền
void veNenDocTruc(float x, float y, float z, float R, float alpha)
{
	// Vẽ lục giác
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_QUADS);
	glColor4f(1.0, 0.2, 1.0, alpha);
	glNormal3f(1.0f, 1.0f, 1.0f);
	//đỉnh 1
	glVertex3f(x + R, y, z);
	//Đỉnh 2
	glVertex3f(x + R * sin(30 * PI / 180), y, z + R * cos(30 * PI / 180));
	//Đỉnh 3
	glVertex3f(x - R * sin(30 * PI / 180), y, z + R * cos(30 * PI / 180));
	//Đỉnh 4
	glVertex3f(x - R, y, z);
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_QUADS);
	glColor4f(1.0, 0.2, 1.0, alpha);
	glNormal3f(1.0f, 1.0f, 1.0f);
	//đỉnh 1
	glVertex3f(x + R, y, z);
	//Đỉnh 6
	glVertex3f(x + R * sin(30 * PI / 180), y, z - R * cos(30 * PI / 180));
	//Đỉnh 5
	glVertex3f(x - R * sin(30 * PI / 180), y, z - R * cos(30 * PI / 180));
	//Đỉnh 4
	glVertex3f(x - R, y, z);
	glEnd();
	//////////tam giác 1 ///////////////////
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_POLYGON);
	glColor4f(1.0, 0.6, 0.2, alpha);
	glNormal3f(1.0f, 1.0f, 1.0f);
	//đỉnh 1
	glVertex3f(x + R, y, z);
	//đỉnh 8
	glVertex3f(x + R + R * cos(30 * PI / 180), y, z - R / 2);
	//đỉnh 9
	glVertex3f(x + R + R * cos(30 * PI / 180), y, z + R / 2);
	glEnd();
	//////////tam giác 2 ///////////////////
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_POLYGON);
	glColor4f(1.0, 0.6, 0.2, alpha);
	glNormal3f(1.0f, 1.0f, 1.0f);
	//đỉnh 2
	glVertex3f(x + R * sin(30 * PI / 180), y, z + R * cos(30 * PI / 180));
	//đỉnh 12
	glVertex3f(x + R * sin(30 * PI / 180), y, z + R * cos(30 * PI / 180) + R);
	//đỉnh 11
	glVertex3f(x + R * sin(30 * PI / 180) + R * sin(60 * PI / 180), y, z + R * cos(30 * PI / 180) + R / 2);
	glEnd();
	////////tam giác 3 ///////////////////
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_POLYGON);
	glColor4f(1.0, 0.6, 0.2, alpha);
	glNormal3f(1.0f, 1.0f, 1.0f);
	//đỉnh 3
	glVertex3f(x - R * sin(30 * PI / 180), y, z + R * cos(30 * PI / 180));
	//đỉnh 14
	glVertex3f(x - R * sin(30 * PI / 180), y, z + R * cos(30 * PI / 180) + R);
	//đỉnh 15
	glVertex3f(x - R * sin(30 * PI / 180) - R * sin(60 * PI / 180), y, z + R * cos(30 * PI / 180) + R / 2);
	glEnd();
	//////////////tam giác 4//////////////////////
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_POLYGON);
	glColor4f(1.0, 0.6, 0.2, alpha);
	glNormal3f(1.0f, 1.0f, 1.0f);
	//đỉnh 4
	glVertex3f(x - R, y, z);
	//đỉnh 17
	glVertex3f(x - R - R * cos(30 * PI / 180), y, z - R / 2);
	//đỉnh 18
	glVertex3f(x - R - R * cos(30 * PI / 180), y, z + R / 2);
	glEnd();
	//////////tam giác 5 ///////////////////
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_POLYGON);
	glColor4f(1.0, 0.6, 0.2, alpha);
	glNormal3f(1.0f, 1.0f, 1.0f);
	//đỉnh 5
	glVertex3f(x - R * sin(30 * PI / 180), y, z - R * cos(30 * PI / 180));
	//đỉnh 20
	glVertex3f(x - R * sin(30 * PI / 180), y, z - R * cos(30 * PI / 180) - R);
	//đỉnh 21
	glVertex3f(x - R * sin(30 * PI / 180) - R * sin(60 * PI / 180), y, z - R * cos(30 * PI / 180) - R / 2);
	glEnd();
	//////////tam giác 6 ///////////////////
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_POLYGON);
	glColor4f(1.0, 0.6, 0.2, alpha);
	glNormal3f(1.0f, 1.0f, 1.0f);
	//đỉnh 6
	glVertex3f(x + R * sin(30 * PI / 180), y, z - R * cos(30 * PI / 180));
	//đỉnh 24
	glVertex3f(x + R * sin(30 * PI / 180), y, z - R * cos(30 * PI / 180) - R);
	//đỉnh 23
	glVertex3f(x + R * sin(30 * PI / 180) + R * sin(60 * PI / 180), y, z - R * cos(30 * PI / 180) - R / 2);
	glEnd();
	/////////////////vuông 1//////////////////////
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_POLYGON);
	glColor4f(0.0, 1.0, 0.0, alpha);
	glNormal3f(1.0f, 1.0f, 1.0f);
	//đỉnh 1 ok
	glVertex3f(x + R, y, z);
	//đỉnh 9 ok
	glVertex3f(x + R + R * cos(30 * PI / 180), y, z + R / 2);
	//đỉnh 11 ok
	glVertex3f(x + R * sin(30 * PI / 180) + R * sin(60 * PI / 180), y, z + R * cos(30 * PI / 180) + R / 2);
	//đỉnh 2 ok
	glVertex3f(x + R * sin(30 * PI / 180), y, z + R * cos(30 * PI / 180));
	glEnd();
	////////////////vuông 2 //////////////////////
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_POLYGON);
	glColor4f(0.0, 1.0, 0.0, alpha);
	glNormal3f(1.0f, 1.0f, 1.0f);
	//đỉnh 2
	glVertex3f(x + R * sin(30 * PI / 180), y, z + R * cos(30 * PI / 180));
	//đỉnh 12
	glVertex3f(x + R * sin(30 * PI / 180), y, z + R * cos(30 * PI / 180) + R);
	//đỉnh 14
	glVertex3f(x - R * sin(30 * PI / 180), y, z + R * cos(30 * PI / 180) + R);
	//đỉnh 3
	glVertex3f(x - R * sin(30 * PI / 180), y, z + R * cos(30 * PI / 180));
	glEnd();
	////////////////vuông 3 //////////////////////
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_POLYGON);
	glColor4f(0.0, 1.0, 0.0, alpha);
	glNormal3f(1.0f, 1.0f, 1.0f);
	//đỉnh 3
	glVertex3f(x - R * sin(30 * PI / 180), y, z + R * cos(30 * PI / 180));
	//đỉnh 4
	glVertex3f(x - R, y, z);
	//đỉnh 18
	glVertex3f(x - R - R * cos(30 * PI / 180), y, z + R / 2);
	//đỉnh 15
	glVertex3f(x - R * sin(30 * PI / 180) - R * sin(60 * PI / 180), y, z + R * cos(30 * PI / 180) + R / 2);
	glEnd();
	////////////////vuông 4 //////////////////////
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_POLYGON);
	glColor4f(0.0, 1.0, 0.0, alpha);
	glNormal3f(1.0f, 1.0f, 1.0f);
	//đỉnh 4
	glVertex3f(x - R, y, z);
	//đỉnh 5
	glVertex3f(x - R * sin(30 * PI / 180), y, z - R * cos(30 * PI / 180));
	//đỉnh 21
	glVertex3f(x - R * sin(30 * PI / 180) - R * sin(60 * PI / 180), y, z - R * cos(30 * PI / 180) - R / 2);
	//đỉnh 17
	glVertex3f(x - R - R * cos(30 * PI / 180), y, z - R / 2);
	glEnd();
	////////////////vuông 5 //////////////////////
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_POLYGON);
	glColor4f(0.0, 1.0, 0.0, alpha);
	glNormal3f(1.0f, 1.0f, 1.0f);
	//đỉnh 5
	glVertex3f(x - R * sin(30 * PI / 180), y, z - R * cos(30 * PI / 180));
	//đỉnh 20
	glVertex3f(x - R * sin(30 * PI / 180), y, z - R * cos(30 * PI / 180) - R);
	//đỉnh 24
	glVertex3f(x + R * sin(30 * PI / 180), y, z - R * cos(30 * PI / 180) - R);
	//đỉnh 6
	glVertex3f(x + R * sin(30 * PI / 180), y, z - R * cos(30 * PI / 180));
	glEnd();
	////////////////vuông 6 //////////////////////
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_POLYGON);
	glColor4f(0.0, 1.0, 0.0, alpha);
	glNormal3f(1.0f, 1.0f, 1.0f);
	//đỉnh 6
	glVertex3f(x + R * sin(30 * PI / 180), y, z - R * cos(30 * PI / 180));
	//đỉnh 23
	glVertex3f(x + R * sin(30 * PI / 180) + R * sin(60 * PI / 180), y, z - R * cos(30 * PI / 180) - R / 2);
	//đỉnh 8
	glVertex3f(x + R + R * cos(30 * PI / 180), y, z - R / 2);
	//đỉnh 1
	glVertex3f(x + R, y, z);
	glEnd();
}

// vẽ nen
void veNen(float alpha)
{
	float y = 0;
	glDisable(GL_LIGHTING);
	glColor3f(1.0f, 1.0f, 1.0f);
	float d = 0.7, R = d / cos(PI / 6);
	int i = 0;
	for (float x = -30; x < 30; x += R + R * sin(30 * PI / 180) + R * sin(60 * PI / 180))
	{
		float z = (i % 2 == 0) ? -20 : (-20 - cos(30 * PI / 180) * R - R / 2);
		for (; z < 20; z += cos(30 * PI / 180) * 2 * R + R)
			veNenDocTruc(x, y, z, R, alpha);
		i++;
	}
	glEnable(GL_LIGHTING);
}

void drawAxis()
{
	glColor3f(0, 0, 1);
	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0); // x
	glVertex3f(4, 0, 0);

	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0); // y
	glVertex3f(0, 4, 0);

	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0); // z
	glVertex3f(0, 0, 4);
	glEnd();
}

void drawcanhQuat1_1()
{
	glPushMatrix();
	glTranslated(0, KCtoiTam + banKinhBoTronCanhQuat, 0);
	glRotatef(banxoay.rotateY, 0, 0, 1);

	GLfloat diffuse[] = {0.0, 1.0, 0.0, 1.0}; // màu
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	canhQuat1_1.setupMaterial(ambient, diffuse, specular, shininess);

	if (cheDoLuoi)
	{
		canhQuat1_1.DrawWireframe();
	}
	else
	{
		canhQuat1_1.Draw();
	}

	glPopMatrix();
}
void drawcanhQuat1_2()
{
	glPushMatrix();
	// Dời trục
	glTranslated(0, KCtoiTam + banKinhBoTronCanhQuat, 0);
	glRotatef(120.0 + banxoay.rotateY, 0, 0, 1);

	GLfloat diffuse[] = {0.0, 1.0, 0.0, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	canhQuat1_2.setupMaterial(ambient, diffuse, specular, shininess);

	if (cheDoLuoi)
		canhQuat1_2.DrawWireframe();
	else
		canhQuat1_2.Draw();

	glPopMatrix();
}
void drawcanhQuat1_3()
{
	glPushMatrix();
	// Dời trục
	glTranslated(0, KCtoiTam + banKinhBoTronCanhQuat, 0);
	glRotatef(-120.0 + banxoay.rotateY, 0, 0, 1);

	GLfloat diffuse[] = {0.0, 1.0, 0.0, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	canhQuat1_2.setupMaterial(ambient, diffuse, specular, shininess);

	if (cheDoLuoi)
		canhQuat1_2.DrawWireframe();
	else
		canhQuat1_2.Draw();

	glPopMatrix();
}

float doDoiCanhQuat2X = 0.2;
float doDoiCanhQuat2Y = KCtoiTam + banKinhBoTronCanhQuat + 1 + 0.3;
float doDoiCanhQuat2Z = 2 * dayCanhQuat;
void drawcanhQuat2_1()
{
	glPushMatrix();
	glTranslated(doDoiCanhQuat2X, doDoiCanhQuat2Y, doDoiCanhQuat2Z);
	glRotatef(banxoay.rotateY, 0, 0, 1);
	GLfloat diffuse[] = {1.0, 1.0, 0.0, 1.0}; // màu
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	canhQuat1_1.setupMaterial(ambient, diffuse, specular, shininess);

	if (cheDoLuoi)
		canhQuat1_1.DrawWireframe();
	else
		canhQuat1_1.Draw();

	glPopMatrix();
}
void drawcanhQuat2_2()
{
	glPushMatrix();
	// Dời trục
	glTranslated(0.2, KCtoiTam + banKinhBoTronCanhQuat + 1 + 0.3, 2 * dayCanhQuat);
	glRotatef(120.0 + banxoay.rotateY, 0, 0, 1);

	GLfloat diffuse[] = {1.0, 1.0, 0.0, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	canhQuat1_2.setupMaterial(ambient, diffuse, specular, shininess);

	if (cheDoLuoi)
		canhQuat1_2.DrawWireframe();
	else
		canhQuat1_2.Draw();

	glPopMatrix();
}
void drawcanhQuat2_3()
{
	glPushMatrix();
	// Dời trục
	glTranslated(0.2, KCtoiTam + banKinhBoTronCanhQuat + 1 + 0.3, 2 * dayCanhQuat);
	glRotatef(-120.0 + banxoay.rotateY, 0, 0, 1);

	GLfloat diffuse[] = {1.0, 1.0, 0.0, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	canhQuat1_3.setupMaterial(ambient, diffuse, specular, shininess);

	if (cheDoLuoi)
		canhQuat1_3.DrawWireframe();
	else
		canhQuat1_3.Draw();

	glPopMatrix();
}
float doDoiCanhQuat3X = banKinhBoTronCanhQuat / cos(10 * PI / 180) + caoCanhQuat / tan(80 * PI / 180); //
float doDoiCanhQuat3Y = tan(30 * PI / 180) * doDoiCanhQuat3X + doDoiCanhQuat3X / sin(60 * PI / 180);
float doDoiCanhQuat3Z = 2 * dayCanhQuat;
float doDaiLK2 = doDoiCanhQuat3X / (cos(atan(doDoiCanhQuat3Y / doDoiCanhQuat3X))); // Khoảng cách tâm  liên kết 2 lỗ 2
void drawcanhQuat3_1()
{
	glPushMatrix();
	glTranslated(doDoiCanhQuat3X, KCtoiTam + banKinhBoTronCanhQuat + doDoiCanhQuat3Y, -doDoiCanhQuat3Z);
	glRotatef(banxoay.rotateY, 0, 0, 1);

	GLfloat diffuse[] = {1.0, 0.0, 0.0, 1.0}; // màu
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	canhQuat1_1.setupMaterial(ambient, diffuse, specular, shininess);

	if (cheDoLuoi)
		canhQuat1_1.DrawWireframe();
	else
		canhQuat1_1.Draw();

	glPopMatrix();
}
void drawcanhQuat3_2()
{
	glPushMatrix();
	// Dời trục
	glTranslated(doDoiCanhQuat3X, KCtoiTam + banKinhBoTronCanhQuat + doDoiCanhQuat3Y, -2 * dayCanhQuat);
	glRotatef(120.0 + banxoay.rotateY, 0, 0, 1);

	GLfloat diffuse[] = {1.0, 0.0, 0.0, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	canhQuat1_2.setupMaterial(ambient, diffuse, specular, shininess);

	if (cheDoLuoi)
		canhQuat1_2.DrawWireframe();
	else
		canhQuat1_2.Draw();

	glPopMatrix();
}
void drawcanhQuat3_3()
{
	glPushMatrix();
	// Dời trục
	glTranslated(doDoiCanhQuat3X, KCtoiTam + banKinhBoTronCanhQuat + doDoiCanhQuat3Y, -2 * dayCanhQuat);
	glRotatef(-120.0 + banxoay.rotateY, 0, 0, 1);

	GLfloat diffuse[] = {1.0, 0.0, 0.0, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	canhQuat1_3.setupMaterial(ambient, diffuse, specular, shininess);

	if (cheDoLuoi)
		canhQuat1_3.DrawWireframe();
	else
		canhQuat1_3.Draw();

	glPopMatrix();
}

// gối đỡ 1
void veGoiDo1()
{
	glPushMatrix();
	// Dời trục
	glTranslated(doDoiCanhQuat3X, KCtoiTam + banKinhBoTronCanhQuat + doDoiCanhQuat3Y, -5 * dayCanhQuat);

	GLfloat diffuse[] = {0.0, 0.0, 1.0, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	goiDo.setupMaterial(ambient, diffuse, specular, shininess);
	if (cheDoLuoi)
		goiDo.DrawWireframe();
	else
		goiDo.Draw();

	glPopMatrix();
}
void veGoiDo2()
{
	glPushMatrix();
	// Dời trục
	glTranslated(doDoiCanhQuat3X, KCtoiTam + banKinhBoTronCanhQuat + doDoiCanhQuat3Y, -5 * dayCanhQuat);
	glRotatef(-45, 0, 0, 1);

	GLfloat diffuse[] = {0.0, 0.0, 1.0, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	goiDo.setupMaterial(ambient, diffuse, specular, shininess);
	if (cheDoLuoi)
		goiDo.DrawWireframe();
	else
		goiDo.Draw();

	glPopMatrix();
}
void veGoiDo3()
{
	glPushMatrix();
	// Dời trục
	glTranslated(doDoiCanhQuat3X, KCtoiTam + banKinhBoTronCanhQuat + doDoiCanhQuat3Y, -5 * dayCanhQuat);
	glRotatef(45, 0, 0, 1);

	GLfloat diffuse[] = {0.0, 0.0, 1.0, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	goiDo.setupMaterial(ambient, diffuse, specular, shininess);
	if (cheDoLuoi)
		goiDo.DrawWireframe();
	else
		goiDo.Draw();

	glPopMatrix();
}
void veGoiDo4()
{
	glPushMatrix();
	// Dời trục
	glTranslated(doDoiCanhQuat3X, KCtoiTam + banKinhBoTronCanhQuat + doDoiCanhQuat3Y, -5 * dayCanhQuat);
	glRotatef(90, 0, 0, 1);

	GLfloat diffuse[] = {0.0, 0.0, 1.0, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	goiDo.setupMaterial(ambient, diffuse, specular, shininess);
	if (cheDoLuoi)
		goiDo.DrawWireframe();
	else
		goiDo.Draw();

	glPopMatrix();
}
void veGoiDo5()
{
	glPushMatrix();
	// Dời trục
	glTranslated(doDoiCanhQuat3X, KCtoiTam + banKinhBoTronCanhQuat + doDoiCanhQuat3Y, -5 * dayCanhQuat);
	glRotatef(-90, 0, 0, 1);

	GLfloat diffuse[] = {0.0, 0.0, 1.0, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	goiDo.setupMaterial(ambient, diffuse, specular, shininess);
	if (cheDoLuoi)
		goiDo.DrawWireframe();
	else
		goiDo.Draw();

	glPopMatrix();
}
void veGoiDo6()
{
	glPushMatrix();
	// Dời trục
	glTranslated(doDoiCanhQuat3X, KCtoiTam + banKinhBoTronCanhQuat + doDoiCanhQuat3Y, -5 * dayCanhQuat);
	glRotatef(-135, 0, 0, 1);

	GLfloat diffuse[] = {0.0, 0.0, 1.0, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	goiDo.setupMaterial(ambient, diffuse, specular, shininess);
	if (cheDoLuoi)
		goiDo.DrawWireframe();
	else
		goiDo.Draw();

	glPopMatrix();
}
void veGoiDo7()
{
	glPushMatrix();
	// Dời trục
	glTranslated(doDoiCanhQuat3X, KCtoiTam + banKinhBoTronCanhQuat + doDoiCanhQuat3Y, -5 * dayCanhQuat);
	glRotatef(135, 0, 0, 1);

	GLfloat diffuse[] = {0.0, 0.0, 1.0, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	goiDo.setupMaterial(ambient, diffuse, specular, shininess);
	if (cheDoLuoi)
		goiDo.DrawWireframe();
	else
		goiDo.Draw();

	glPopMatrix();
}
void veGoiDo8()
{
	glPushMatrix();
	// Dời trục
	glTranslated(doDoiCanhQuat3X, KCtoiTam + banKinhBoTronCanhQuat + doDoiCanhQuat3Y, -5 * dayCanhQuat);
	glRotatef(180, 0, 0, 1);

	GLfloat diffuse[] = {0.0, 0.0, 1.0, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	goiDo.setupMaterial(ambient, diffuse, specular, shininess);
	if (cheDoLuoi)
		goiDo.DrawWireframe();
	else
		goiDo.Draw();

	glPopMatrix();
}
void veLK2lo11()
{
	glPushMatrix();
	// Dời trục
	glTranslated(cos(banxoay.rotateY * PI / 180 + PI / 2) * (KC1 + caoCanhQuat) + cos(atan(1.3 / 0.2)) * 0.2 / (cos(atan(1.3 / 0.2)) * 2), KCtoiTam + banKinhBoTronCanhQuat - doDaiLK / 2 + doDaiLK / 2 + sin(atan(1.3 / 0.2)) * 0.2 / (cos(atan(1.3 / 0.2)) * 2) + sin(banxoay.rotateY * PI / 180 + PI / 2) * (KC1 + caoCanhQuat), dayCanhQuat);
	glRotatef(-atan(0.2 / 1.3) * 180 / PI, 0, 0, 1);
	GLfloat diffuse[] = {1.0, 0.7, 0.4, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	LK2lo1.setupMaterial(ambient, diffuse, specular, shininess);
	if (cheDoLuoi)
		LK2lo1.DrawWireframe();
	else
		LK2lo1.Draw();

	glPopMatrix();
}
void veLK2lo12()
{
	glPushMatrix();
	// Dời trục
	float doiTrucX = cos(PI / 2) * (KC1 + caoCanhQuat) + cos(atan(1.3 / 0.2)) * 0.2 / (cos(atan(1.3 / 0.2)) * 2);
	glTranslated(doiTrucX + cos(30 * PI / 180 - banxoay.rotateY * PI / 180) * KCtoiTam, -KCtoiTam * sin(30 * PI / 180 - banxoay.rotateY * PI / 180) + banKinhBoTronCanhQuat - doDaiLK / 2 + doDaiLK / 2 + sin(atan(1.3 / 0.2)) * 0.2 / (cos(atan(1.3 / 0.2)) * 2) + sin(+PI / 2) * (KC1 + caoCanhQuat), dayCanhQuat);
	glRotatef(-atan(0.2 / 1.3) * 180 / PI, 0, 0, 1);
	GLfloat diffuse[] = {1.0, 0.7, 0.4, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	LK2lo1.setupMaterial(ambient, diffuse, specular, shininess);
	if (cheDoLuoi)
		LK2lo1.DrawWireframe();
	else
		LK2lo1.Draw();

	glPopMatrix();
}
void veLK2lo13()
{
	glPushMatrix();
	// Dời trục
	float doiTrucX = cos(PI / 2) * (KC1 + caoCanhQuat) + cos(atan(1.3 / 0.2)) * 0.2 / (cos(atan(1.3 / 0.2)) * 2);
	glTranslated(doiTrucX - cos(30 * PI / 180 + banxoay.rotateY * PI / 180) * KCtoiTam, -KCtoiTam * sin(30 * PI / 180 + banxoay.rotateY * PI / 180) + banKinhBoTronCanhQuat - doDaiLK / 2 + doDaiLK / 2 + sin(atan(1.3 / 0.2)) * 0.2 / (cos(atan(1.3 / 0.2)) * 2) + sin(+PI / 2) * (KC1 + caoCanhQuat), dayCanhQuat);
	glRotatef(-atan(0.2 / 1.3) * 180 / PI, 0, 0, 1);
	GLfloat diffuse[] = {1.0, 0.7, 0.4, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	LK2lo1.setupMaterial(ambient, diffuse, specular, shininess);
	if (cheDoLuoi)
		LK2lo1.DrawWireframe();
	else
		LK2lo1.Draw();

	glPopMatrix();
}
void veLK2lo14()
{
	glPushMatrix();
	// Dời trục
	glTranslated(cos(banxoay.rotateY * PI / 180 + PI / 2) * (KC1 + caoCanhQuat) + cos(atan(doDoiCanhQuat3Y / doDoiCanhQuat3X)) * doDoiCanhQuat3X / (cos(atan(doDoiCanhQuat3Y / doDoiCanhQuat3X)) * 2), KCtoiTam + banKinhBoTronCanhQuat - doDaiLK / 2 + doDaiLK / 2 + sin(atan(doDoiCanhQuat3Y / doDoiCanhQuat3X)) * doDoiCanhQuat3X / (cos(atan(doDoiCanhQuat3Y / doDoiCanhQuat3X)) * 2) + sin(banxoay.rotateY * PI / 180 + PI / 2) * (KC1 + caoCanhQuat), -dayCanhQuat);
	glRotatef(-atan(doDoiCanhQuat3X / doDoiCanhQuat3Y) * 180 / PI, 0, 0, 1);
	GLfloat diffuse[] = {1.0, 0.7, 0.4, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	LK2lo2.setupMaterial(ambient, diffuse, specular, shininess);
	if (cheDoLuoi)
		LK2lo2.DrawWireframe();
	else
		LK2lo2.Draw();

	glPopMatrix();
}
void veLK2lo15()
{
	glPushMatrix();
	// Dời trục
	float doiTrucX = cos(PI / 2) * (KC1 + caoCanhQuat) + cos(atan(doDoiCanhQuat3Y / doDoiCanhQuat3X)) * doDoiCanhQuat3X / (cos(atan(doDoiCanhQuat3Y / doDoiCanhQuat3X)) * 2);
	glTranslated(doiTrucX + cos(30 * PI / 180 - banxoay.rotateY * PI / 180) * KCtoiTam, -KCtoiTam * sin(30 * PI / 180 - banxoay.rotateY * PI / 180) + banKinhBoTronCanhQuat - doDaiLK / 2 + doDaiLK / 2 + sin(atan(doDoiCanhQuat3Y / doDoiCanhQuat3X)) * doDoiCanhQuat3X / (cos(atan(doDoiCanhQuat3Y / doDoiCanhQuat3X)) * 2) + sin(+PI / 2) * (KC1 + caoCanhQuat), -dayCanhQuat);
	glRotatef(-atan(doDoiCanhQuat3X / doDoiCanhQuat3Y) * 180 / PI, 0, 0, 1);
	GLfloat diffuse[] = {1.0, 0.7, 0.4, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	LK2lo2.setupMaterial(ambient, diffuse, specular, shininess);
	if (cheDoLuoi)
		LK2lo2.DrawWireframe();
	else
		LK2lo2.Draw();

	glPopMatrix();
}
void veLK2lo16()
{
	glPushMatrix();
	// Dời trục
	float doiTrucX = cos(PI / 2) * (KC1 + caoCanhQuat) + cos(atan(doDoiCanhQuat3Y / doDoiCanhQuat3X)) * doDoiCanhQuat3X / (cos(atan(doDoiCanhQuat3Y / doDoiCanhQuat3X)) * 2);
	glTranslated(doiTrucX - cos(30 * PI / 180 + banxoay.rotateY * PI / 180) * KCtoiTam, -KCtoiTam * sin(30 * PI / 180 + banxoay.rotateY * PI / 180) + banKinhBoTronCanhQuat - doDaiLK / 2 + doDaiLK / 2 + sin(atan(doDoiCanhQuat3Y / doDoiCanhQuat3X)) * doDoiCanhQuat3X / (cos(atan(doDoiCanhQuat3Y / doDoiCanhQuat3X)) * 2) + sin(+PI / 2) * (KC1 + caoCanhQuat), -dayCanhQuat);
	glRotatef(-atan(doDoiCanhQuat3X / doDoiCanhQuat3Y) * 180 / PI, 0, 0, 1);
	GLfloat diffuse[] = {1.0, 0.7, 0.4, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	LK2lo2.setupMaterial(ambient, diffuse, specular, shininess);
	if (cheDoLuoi)
		LK2lo2.DrawWireframe();
	else
		LK2lo2.Draw();

	glPopMatrix();
}
// gối đỡ 2
void veGoiDo9()
{
	glPushMatrix();
	// Dời trục
	glTranslated(doDoiCanhQuat2X, doDoiCanhQuat2Y, doDoiCanhQuat2Z + 3 * dayCanhQuat);

	GLfloat diffuse[] = {0.0, 0.0, 1.0, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	goiDo.setupMaterial(ambient, diffuse, specular, shininess);
	if (cheDoLuoi)
		goiDo.DrawWireframe();
	else
		goiDo.Draw();

	glPopMatrix();
}
void veGoiDo10()
{
	glPushMatrix();
	// Dời trục
	glTranslated(doDoiCanhQuat2X, doDoiCanhQuat2Y, doDoiCanhQuat2Z + 3 * dayCanhQuat);
	glRotatef(-45, 0, 0, 1);

	GLfloat diffuse[] = {0.0, 0.0, 1.0, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	goiDo.setupMaterial(ambient, diffuse, specular, shininess);
	if (cheDoLuoi)
		goiDo.DrawWireframe();
	else
		goiDo.Draw();

	glPopMatrix();
}
void veGoiDo11()
{
	glPushMatrix();
	// Dời trục
	glTranslated(doDoiCanhQuat2X, doDoiCanhQuat2Y, doDoiCanhQuat2Z + 3 * dayCanhQuat);
	glRotatef(45, 0, 0, 1);

	GLfloat diffuse[] = {0.0, 0.0, 1.0, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	goiDo.setupMaterial(ambient, diffuse, specular, shininess);
	if (cheDoLuoi)
		goiDo.DrawWireframe();
	else
		goiDo.Draw();

	glPopMatrix();
}
void veGoiDo12()
{
	glPushMatrix();
	// Dời trục
	glTranslated(doDoiCanhQuat2X, doDoiCanhQuat2Y, doDoiCanhQuat2Z + 3 * dayCanhQuat);
	glRotatef(90, 0, 0, 1);

	GLfloat diffuse[] = {0.0, 0.0, 1.0, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	goiDo.setupMaterial(ambient, diffuse, specular, shininess);
	if (cheDoLuoi)
		goiDo.DrawWireframe();
	else
		goiDo.Draw();

	glPopMatrix();
}
void veGoiDo13()
{
	glPushMatrix();
	// Dời trục
	glTranslated(doDoiCanhQuat2X, doDoiCanhQuat2Y, doDoiCanhQuat2Z + 3 * dayCanhQuat);
	glRotatef(-90, 0, 0, 1);

	GLfloat diffuse[] = {0.0, 0.0, 1.0, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	goiDo.setupMaterial(ambient, diffuse, specular, shininess);
	if (cheDoLuoi)
		goiDo.DrawWireframe();
	else
		goiDo.Draw();

	glPopMatrix();
}
void veGoiDo14()
{
	glPushMatrix();
	// Dời trục
	glTranslated(doDoiCanhQuat2X, doDoiCanhQuat2Y, doDoiCanhQuat2Z + 3 * dayCanhQuat);
	glRotatef(-135, 0, 0, 1);

	GLfloat diffuse[] = {0.0, 0.0, 1.0, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	goiDo.setupMaterial(ambient, diffuse, specular, shininess);
	if (cheDoLuoi)
		goiDo.DrawWireframe();
	else
		goiDo.Draw();

	glPopMatrix();
}
void veGoiDo15()
{
	glPushMatrix();
	// Dời trục
	glTranslated(doDoiCanhQuat2X, doDoiCanhQuat2Y, doDoiCanhQuat2Z + 3 * dayCanhQuat);
	glRotatef(135, 0, 0, 1);

	GLfloat diffuse[] = {0.0, 0.0, 1.0, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	goiDo.setupMaterial(ambient, diffuse, specular, shininess);
	if (cheDoLuoi)
		goiDo.DrawWireframe();
	else
		goiDo.Draw();

	glPopMatrix();
}
void veGoiDo16()
{
	glPushMatrix();
	// Dời trục
	glTranslated(doDoiCanhQuat2X, doDoiCanhQuat2Y, doDoiCanhQuat2Z + 3 * dayCanhQuat);
	glRotatef(180, 0, 0, 1);

	GLfloat diffuse[] = {0.0, 0.0, 1.0, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	goiDo.setupMaterial(ambient, diffuse, specular, shininess);
	if (cheDoLuoi)
		goiDo.DrawWireframe();
	else
		goiDo.Draw();

	glPopMatrix();
}

void veTrucTo1()
{
	glPushMatrix();

	glTranslated(doDoiCanhQuat3X, KCtoiTam + banKinhBoTronCanhQuat + doDoiCanhQuat3Y, -2.5 * dayCanhQuat - trucToDai / 2);
	glRotatef(banxoay.rotateY, 0, 0, 1);

	GLfloat diffuse[] = {0.5, 0.5, 0.5, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	chot.setupMaterial(ambient, diffuse, specular, shininess);

	if (cheDoLuoi)
		chot.DrawWireframe();
	else
		chot.Draw();

	glPopMatrix();
}

void veTrucTo2()
{
	glPushMatrix();

	glTranslated(doDoiCanhQuat2X, doDoiCanhQuat2Y, doDoiCanhQuat2Z + trucToDai / 2 + 0.5 * dayCanhQuat);
	glRotatef(banxoay.rotateY, 0, 0, 1);

	GLfloat diffuse[] = {0.5, 0.5, 0.5, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	chot.setupMaterial(ambient, diffuse, specular, shininess);

	if (cheDoLuoi)
		chot.DrawWireframe();
	else
		chot.Draw();

	glPopMatrix();
}

// chốt màu hồng cánh quạt 1
void veChotLKMauHong1()
{
	glPushMatrix();

	glTranslated(cos((90 + banxoay.rotateY) * PI / 180) * KCtoiTam, sin((90 + banxoay.rotateY) * PI / 180) * KCtoiTam + banKinhBoTronCanhQuat + KC1 + caoCanhQuat, dayCanhQuat);

	GLfloat diffuse[] = {1.0, 0.2, 0.6, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	chotMauHong.setupMaterial(ambient, diffuse, specular, shininess);

	if (cheDoLuoi)
		chotMauHong.DrawWireframe();
	else
		chotMauHong.Draw();

	glPopMatrix();
}
void veChotLKMauHong2()
{
	glPushMatrix();

	glTranslated(cos((90 + banxoay.rotateY) * PI / 180) * KCtoiTam, sin((90 + banxoay.rotateY) * PI / 180) * KCtoiTam + banKinhBoTronCanhQuat + KC1 + caoCanhQuat, -dayCanhQuat);
	GLfloat diffuse[] = {1.0, 0.2, 0.6, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	chotMauHong.setupMaterial(ambient, diffuse, specular, shininess);

	if (cheDoLuoi)
		chotMauHong.DrawWireframe();
	else
		chotMauHong.Draw();

	glPopMatrix();
}
void veChotLKMauHong3()
{
	glPushMatrix();
	glTranslated(cos((210 + banxoay.rotateY) * PI / 180) * KCtoiTam, sin((210 + banxoay.rotateY) * PI / 180) * KCtoiTam + banKinhBoTronCanhQuat + KC1 + caoCanhQuat, dayCanhQuat);

	GLfloat diffuse[] = {1.0, 0.2, 0.6, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	chotMauHong.setupMaterial(ambient, diffuse, specular, shininess);

	if (cheDoLuoi)
		chotMauHong.DrawWireframe();
	else
		chotMauHong.Draw();
	glPopMatrix();
}
void veChotLKMauHong4()
{
	glPushMatrix();
	glTranslated(cos((210 + banxoay.rotateY) * PI / 180) * KCtoiTam, sin((210 + banxoay.rotateY) * PI / 180) * KCtoiTam + banKinhBoTronCanhQuat + KC1 + caoCanhQuat, -dayCanhQuat);
	GLfloat diffuse[] = {1.0, 0.2, 0.6, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	chotMauHong.setupMaterial(ambient, diffuse, specular, shininess);
	if (cheDoLuoi)
		chotMauHong.DrawWireframe();
	else
		chotMauHong.Draw();
	glPopMatrix();
}
void veChotLKMauHong5()
{
	glPushMatrix();
	glTranslated(cos((-30 + banxoay.rotateY) * PI / 180) * KCtoiTam, sin((-30 + banxoay.rotateY) * PI / 180) * KCtoiTam + banKinhBoTronCanhQuat + KC1 + caoCanhQuat, dayCanhQuat);

	GLfloat diffuse[] = {1.0, 0.2, 0.6, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	chotMauHong.setupMaterial(ambient, diffuse, specular, shininess);
	if (cheDoLuoi)
		chotMauHong.DrawWireframe();
	else
		chotMauHong.Draw();
	glPopMatrix();
}
void veChotLKMauHong6()
{
	glPushMatrix();
	glTranslated(cos((-30 + banxoay.rotateY) * PI / 180) * KCtoiTam, sin((-30 + banxoay.rotateY) * PI / 180) * KCtoiTam + banKinhBoTronCanhQuat + KC1 + caoCanhQuat, -dayCanhQuat);
	GLfloat diffuse[] = {1.0, 0.2, 0.6, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	chotMauHong.setupMaterial(ambient, diffuse, specular, shininess);
	if (cheDoLuoi)
	{
		chotMauHong.DrawWireframe();
	}
	else
	{
		chotMauHong.Draw();
	}
	glPopMatrix();
}
// chốt màu hồng cánh quạt 2
void veChotLKMauHong7()
{
	glPushMatrix();
	glTranslated(cos((90 + banxoay.rotateY) * PI / 180) * KCtoiTam + doDoiCanhQuat2X, sin((90 + banxoay.rotateY) * PI / 180) * KCtoiTam + KC1 + caoCanhQuat + doDoiCanhQuat2Y - KCtoiTam, dayCanhQuat);
	GLfloat diffuse[] = {1.0, 0.2, 0.6, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	chotMauHong.setupMaterial(ambient, diffuse, specular, shininess);
	if (cheDoLuoi)
	{
		chotMauHong.DrawWireframe();
	}
	else
	{
		chotMauHong.Draw();
	}

	glPopMatrix();
}
void veChotLKMauHong8()
{
	glPushMatrix();
	glTranslated(cos((210 + banxoay.rotateY) * PI / 180) * KCtoiTam + doDoiCanhQuat2X, sin((210 + banxoay.rotateY) * PI / 180) * KCtoiTam + KC1 + caoCanhQuat + doDoiCanhQuat2Y - KCtoiTam, dayCanhQuat);
	GLfloat diffuse[] = {1.0, 0.2, 0.6, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	chotMauHong.setupMaterial(ambient, diffuse, specular, shininess);
	if (cheDoLuoi)
	{
		chotMauHong.DrawWireframe();
	}
	else
	{
		chotMauHong.Draw();
	}

	glPopMatrix();
}
void veChotLKMauHong9()
{
	glPushMatrix();
	glTranslated(cos((-30 + banxoay.rotateY) * PI / 180) * KCtoiTam + doDoiCanhQuat2X, sin((-30 + banxoay.rotateY) * PI / 180) * KCtoiTam + KC1 + caoCanhQuat + doDoiCanhQuat2Y - KCtoiTam, dayCanhQuat);

	GLfloat diffuse[] = {1.0, 0.2, 0.6, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	chotMauHong.setupMaterial(ambient, diffuse, specular, shininess);

	if (cheDoLuoi)
	{
		chotMauHong.DrawWireframe();
	}
	else
	{
		chotMauHong.Draw();
	}

	glPopMatrix();
}
// chốt màu hồng cánh quạt 3
void veChotLKMauHong10()
{
	glPushMatrix();
	glTranslated(cos((90 + banxoay.rotateY) * PI / 180) * KCtoiTam + doDoiCanhQuat3X, sin((90 + banxoay.rotateY) * PI / 180) * KCtoiTam + KC1 + caoCanhQuat + banKinhBoTronCanhQuat + doDoiCanhQuat3Y, -dayCanhQuat);
	GLfloat diffuse[] = {1.0, 0.2, 0.6, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	chotMauHong.setupMaterial(ambient, diffuse, specular, shininess);

	if (cheDoLuoi)
	{
		chotMauHong.DrawWireframe();
	}
	else
	{
		chotMauHong.Draw();
	}

	glPopMatrix();
}
void veChotLKMauHong11()
{
	glPushMatrix();
	glTranslated(cos((210 + banxoay.rotateY) * PI / 180) * KCtoiTam + doDoiCanhQuat3X, sin((210 + banxoay.rotateY) * PI / 180) * KCtoiTam + KC1 + caoCanhQuat + banKinhBoTronCanhQuat + doDoiCanhQuat3Y, -dayCanhQuat);
	GLfloat diffuse[] = {1.0, 0.2, 0.6, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	chotMauHong.setupMaterial(ambient, diffuse, specular, shininess);
	if (cheDoLuoi)
	{
		chotMauHong.DrawWireframe();
	}
	else
	{
		chotMauHong.Draw();
	}
	glPopMatrix();
}
void veChotLKMauHong12()
{
	glPushMatrix();

	glTranslated(cos((-30 + banxoay.rotateY) * PI / 180) * KCtoiTam + doDoiCanhQuat3X, sin((-30 + banxoay.rotateY) * PI / 180) * KCtoiTam + KC1 + caoCanhQuat + banKinhBoTronCanhQuat + doDoiCanhQuat3Y, -dayCanhQuat);

	GLfloat diffuse[] = {1.0, 0.2, 0.6, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	chotMauHong.setupMaterial(ambient, diffuse, specular, shininess);

	if (cheDoLuoi)
	{
		chotMauHong.DrawWireframe();
	}
	else
	{
		chotMauHong.Draw();
	}

	glPopMatrix();
}

void trucBatGiac1()
{
	glPushMatrix();

	glTranslated(doDoiCanhQuat3X, KCtoiTam + banKinhBoTronCanhQuat + doDoiCanhQuat3Y, -3 * dayCanhQuat - trucToDai / 2 - trucToDai + goiDoDay);
	glRotatef(banxoay.rotateY, 0, 0, 1);

	GLfloat diffuse[] = {0.5, 0.5, 0.5, 1.0};
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	trucBatGiac.setupMaterial(ambient, diffuse, specular, shininess);

	if (cheDoLuoi)
	{
		trucBatGiac.DrawWireframe();
	}
	else
	{
		trucBatGiac.Draw();
	}
	glPopMatrix();
}
void trucBatGiac2()
{
	glPushMatrix();
	glTranslated(doDoiCanhQuat2X, doDoiCanhQuat2Y, 3 * dayCanhQuat + trucToDai / 2 + trucToDai - goiDoDay);
	glRotatef(banxoay.rotateY, 0, 0, 1);
	GLfloat diffuse[] = {0.5, 0.5, 0.5, 1.0}; // set màu
	GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat shininess = 40.0;
	trucBatGiac.setupMaterial(ambient, diffuse, specular, shininess);
	if (cheDoLuoi)
	{
		trucBatGiac.DrawWireframe();
	}
	else
	{
		trucBatGiac.Draw();
	}
	glPopMatrix();
}
//

void drawObjects()
{
	veTrucTo1();
	veTrucTo2();
	// vẽ cánh quạt
	drawcanhQuat1_1();
	drawcanhQuat1_2();
	drawcanhQuat1_3();
	drawcanhQuat2_1();
	drawcanhQuat2_2();
	drawcanhQuat2_3();
	drawcanhQuat3_1();
	drawcanhQuat3_2();
	drawcanhQuat3_3();
	// vẽ gối đỡ
	veGoiDo1();
	veGoiDo2();
	veGoiDo3();
	veGoiDo4();
	veGoiDo5();
	veGoiDo6(); //-135
	veGoiDo7(); // 135
	veGoiDo8(); // 180
	veGoiDo9();
	veGoiDo10();
	veGoiDo11();
	veGoiDo12();
	veGoiDo13();
	veGoiDo14(); //-135
	veGoiDo15(); // 135
	veGoiDo16(); // 180
	// truc bát giác 1
	trucBatGiac1();
	trucBatGiac2();
	// chốt liên kết màu hồng
	veChotLKMauHong1();
	veChotLKMauHong2();
	veChotLKMauHong3();
	veChotLKMauHong4();
	veChotLKMauHong5();
	veChotLKMauHong6();
	veChotLKMauHong7();
	veChotLKMauHong8();
	veChotLKMauHong9();
	veChotLKMauHong10();
	veChotLKMauHong11();
	veChotLKMauHong12();
	// LK 2 lỗ
	veLK2lo11();
	veLK2lo12();
	veLK2lo13();
	veLK2lo14();
	veLK2lo15();
	veLK2lo16();
}

void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	GLfloat light_position0[] = {10.0, 10.0, 10.0, 0.0};
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);

	if (bLight1 == true)
	{
		glEnable(GL_LIGHT1);
		GLfloat diffuse1[] = {1.0, 1.0, 1.0, 1.0};
		GLfloat specular1[] = {1.0, 1.0, 1.0, 1.0};
		GLfloat ambient1[] = {0.0, 0.0, 0.0, 1.0};
		GLfloat position1[] = {-10.0, 10.0, -10.0, 0.0};

		glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
		glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);
		glLightfv(GL_LIGHT1, GL_SPECULAR, specular1);
		glLightfv(GL_LIGHT1, GL_POSITION, position1);
	}
	else
		glDisable(GL_LIGHT1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gocNhinX = khoangCachNhin * sinf(gocNhin * PI / 180);
	gocNhinY = gocNhinLenXuong;
	gocNhinZ = khoangCachNhin * cosf(gocNhin * PI / 180);

	if (khoangCachNhin == 0)
	{
		gluLookAt(gocNhinX, gocNhinY, gocNhinZ, nhinTrucX, nhinTrucY, nhinTrucZ, sinf(gocNhin * PI / 180), 0, cosf(gocNhin * PI / 180));
	}
	else
	{
		gluLookAt(gocNhinX, gocNhinY, gocNhinZ, nhinTrucX, nhinTrucY, nhinTrucZ, 0, 1, 0);
	}

	glViewport(0, 0, screenWidth, screenHeight);
	glClearStencil(0);
	glClearDepth(1.0f);
	drawObjects();
	glDisable(GL_DEPTH_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	veNen(1.0f);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glStencilFunc(GL_EQUAL, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glPushMatrix();
	glScalef(1, -1, 1);
	drawObjects();
	glPopMatrix();
	glDisable(GL_STENCIL_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	veNen(0.7f);
	glDisable(GL_BLEND);
	glFlush();
	glutSwapBuffers();
}

void processTimer(int value)
{
	if (au)
	{
		banxoay.rotateY += 1.5;
		if (banxoay.rotateY > 360)
			banxoay.rotateY -= 360;
	}
	glutTimerFunc(10, processTimer, 0);
	glutPostRedisplay();
}

void initProject()
{
	nhinTrucX = 0;
	nhinTrucY = 2;
	nhinTrucZ = 0;
	gocNhin = 45;
	gocNhinLenXuong = 6;
	khoangCachNhin = 6.5;

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// màu nền

	glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);

	glutTimerFunc(10, processTimer, 0);
	const float ar = (float)screenWidth / (float)screenHeight;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-ar, ar, -1.0, 1.0, 1.5, 50.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat lmodel_ambient[] = {0.0, 0.0, 0.0, 1.0};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

	GLfloat light_ambient0[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat light_diffuse0[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat light_specular0[] = {1.0, 1.0, 1.0, 1.0};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0);
}

void initObjects(int N)
{
	int M = 2 * (N - 1);
	// canhQuat1_1
	canhQuat1_1.veCanhQuat(N, dayCanhQuat, caoCanhQuat, banKinhBoTronCanhQuat);
	// N : chấm điểm vẽ đường tròn N càng lớn thì càng tròn
	canhQuat1_1.CalculateFacesNorm();
	// canhQuat1_2
	canhQuat1_2.veCanhQuat(N, dayCanhQuat, caoCanhQuat, banKinhBoTronCanhQuat);
	canhQuat1_2.CalculateFacesNorm();
	// canhQuat1_3
	canhQuat1_3.veCanhQuat(N, dayCanhQuat, caoCanhQuat, banKinhBoTronCanhQuat);
	canhQuat1_3.CalculateFacesNorm();
	// Vẽ gối đỡ
	goiDo.veGoiDo(N, goiDoDay, trucToBanKinh, trucToBanKinh * 2);
	goiDo.CalculateFacesNorm();
	// Vẽ liên k 2 lỗ loại 1:
	LK2lo1.veLK2lo1(N, dayCanhQuat, doDaiLK, 2 * banKinhBoTronCanhQuat / 3, banKinhBoTronCanhQuat);
	LK2lo1.CalculateFacesNorm();
	// Vẽ liên k 2 lỗ loại 2:
	LK2lo2.veLK2lo1(N, dayCanhQuat, doDaiLK2, 2 * banKinhBoTronCanhQuat / 3, banKinhBoTronCanhQuat);
	LK2lo2.CalculateFacesNorm();
	// truc to 1
	chot.veHinhTru(30, trucToDai, trucToBanKinh);
	// chiều dài , đường kính
	chot.CalculateFacesNorm();
	// truc to 1
	trucBatGiac.veHinhTru(8, trucBatGiacDai - goiDoDay, trucBatGiacBanKinh);
	// chiều dài , đường kính
	trucBatGiac.CalculateFacesNorm();
	// chốt màu tím
	chotMauHong.veHinhTru(N, dayCanhQuat, 2 * banKinhBoTronCanhQuat / 3);
	// chiều dài , đường kính
	chotMauHong.CalculateFacesNorm();
}

void displayOptions()
{
	cout << "1: Xoay canh quat nguoc chieu kim dong ho" << endl
		 << "2: Xoay canh quat cung chieu kim dong ho" << endl
		 << "W, w : Chuyen doi qua lai giua che do khung giay va to mau" << endl
		 << "V, v : Chuyen doi che do nhin" << endl
		 << "+ : Tang giam khoang cach camera" << endl
		 << "- : Giam khoang cach camera" << endl
		 << "up arrow : Tang chieu cao camera" << endl
		 << "down arrow : Giam chieu cao camera" << endl
		 << "<- : Quay camera theo chieu kim dong ho" << endl
		 << "-> : Quay camera nguoc chieu kim dong ho" << endl;
}

int main(int argc, char *argv[])
{
	int N = 19;
	glutInit(&argc, (char **)argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(screenWidth, screenHeight);
	glutInitWindowPosition(150, 200);
	glutCreateWindow("Truong Ngoc Trung Anh - 2020004");
	displayOptions();
	initObjects(N);
	initProject();
	glutKeyboardFunc(pressKeyBoard1);
	glutSpecialFunc(pressKeyBoard2);
	glutDisplayFunc(myDisplay);
	glutMainLoop();
	return 0;
}
