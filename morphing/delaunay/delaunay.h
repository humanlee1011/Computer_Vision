#ifndef H_DELAUNAY
#define H_DELAUNAY

#include "vector2.h"
#include "edge.h"
#include "triangle.h"

#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

template <class T>
class Delaunay
{
	public:
		using TriangleType = Triangle<T>;
		using EdgeType = Edge<T>;
		using VertexType = Vector2<T>;

		const std::vector<TriangleType>& triangulate(std::vector<VertexType> &vertices)
		{
			// Store the vertices locally
			_vertices = vertices;

			// Determinate the super triangle
			T minX = vertices[0].x;
			T minY = vertices[0].y;
			T maxX = minX;
			T maxY = minY;

			for(std::size_t i = 0; i < vertices.size(); ++i)
			{
				if (vertices[i].x < minX) minX = vertices[i].x;
				if (vertices[i].y < minY) minY = vertices[i].y;
				if (vertices[i].x > maxX) maxX = vertices[i].x;
				if (vertices[i].y > maxY) maxY = vertices[i].y;
			}

			const T dx = maxX - minX;
			const T dy = maxY - minY;
			const T deltaMax = std::max(dx, dy);
			const T midx = half(minX + maxX);
			const T midy = half(minY + maxY);

			const VertexType p1(midx - 20 * deltaMax, midy - deltaMax);
			const VertexType p2(midx, midy + 20 * deltaMax);
			const VertexType p3(midx + 20 * deltaMax, midy - deltaMax);

			//std::cout << "Super triangle " << std::endl << Triangle(p1, p2, p3) << std::endl;

			// Create a list of triangles, and add the supertriangle in it
			_triangles.push_back(TriangleType(p1, p2, p3));

			for(auto p = begin(vertices); p != end(vertices); p++)
			{
				//std::cout << "Traitement du point " << *p << std::endl;
				//std::cout << "_triangles contains " << _triangles.size() << " elements" << std::endl;

				std::vector<EdgeType> polygon;

				for(auto & t : _triangles)
				{
					//std::cout << "Processing " << std::endl << *t << std::endl;

					if(t.circumCircleContains(*p))
					{
						//std::cout << "Pushing bad triangle " << *t << std::endl;
						t.isBad = true;
						polygon.push_back(t.e1);
						polygon.push_back(t.e2);
						polygon.push_back(t.e3);
					}
					else
					{
						//std::cout << " does not contains " << *p << " in his circum center" << std::endl;
					}
				}

				_triangles.erase(std::remove_if(begin(_triangles), end(_triangles), [](TriangleType &t){
					return t.isBad;
				}), end(_triangles));

				for(auto e1 = begin(polygon); e1 != end(polygon); ++e1)
				{
					for(auto e2 = e1 + 1; e2 != end(polygon); ++e2)
					{
						if(almost_equal_e(*e1, *e2))
						{
							e1->isBad = true;
							e2->isBad = true;
						}
					}
				}

				polygon.erase(std::remove_if(begin(polygon), end(polygon), [](EdgeType &e){
					return e.isBad;
				}), end(polygon));

				for(const auto e : polygon) {
					int i1 = find(e.p1);
					int i2 = find(e.p2);
					int i3 = find(*p);
					if (i1 <= i2 && i2 <= i3) 
						_triangles.push_back(TriangleType(e.p1, e.p2, *p, i1, i2, i3));
					else if (i1 <= i3 && i3 <= i2)
						_triangles.push_back(TriangleType(e.p1, *p, e.p2, i1, i3, i2));
					else if (i2 <= i1 && i1 <= i3)
						_triangles.push_back(TriangleType(e.p2, e.p1, *p, i2, i1, i3));
					else if (i2 <= i3 && i3 <= i1)
						_triangles.push_back(TriangleType(e.p2, *p, e.p1, i2, i3, i1));					
					else if (i3 <= i1 && i1 <= i2)
						_triangles.push_back(TriangleType(*p, e.p1, e.p2, i3, i1, i2));
					else if (i3 <= i2 && i2 <= i1)
						_triangles.push_back(TriangleType(*p, e.p2, e.p1, i3, i2, i1));

					// cout << "p1: " << e.p1 << endl;
					// cout << "p2: " << e.p2 << endl;
					//cout << "i1: " << i1 << " i2: " << i2 << " i3: " << i3 << endl;
					// if (!(i1 == -1 || i2 == -1 || i3 == -1)) {	
					// _triangles.push_back(TriangleType(e.p1, e.p2, *p, i1, i2, i3));
					// }
				}
			}

			_triangles.erase(std::remove_if(begin(_triangles), end(_triangles), [p1, p2, p3](TriangleType &t){
				return t.containsVertex(p1) || t.containsVertex(p2) || t.containsVertex(p3);
			}), end(_triangles));

			for(const auto t : _triangles)
			{
				_edges.push_back(t.e1);
				_edges.push_back(t.e2);
				_edges.push_back(t.e3);
			}

			return _triangles;
		}
		const int find(const VertexType& p) {
			for (int i = 0; i < _vertices.size(); i++) {
				if (almost_equal_v(p, _vertices[i]))
					return i;
			}
			return -1;
		}

		const std::vector<TriangleType>& getTriangles() const { return _triangles; };
		const std::vector<EdgeType>& getEdges() const { return _edges; };
		const std::vector<VertexType>& getVertices() const { return _vertices; };

	private:
		std::vector<TriangleType> _triangles;
		std::vector<EdgeType> _edges;
		std::vector<VertexType> _vertices;
};

#endif
