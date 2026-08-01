/*
 * The Mailo Today Solutions Software License
 *
 * Copyright 2020 Mailo Today Solutions.
 *
 * No Permission is granted, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software.
 */

#include "DsMeshOperator.h"

namespace ds {

	MeshOperator::MeshOperator() {

	}

	MeshOperator::MeshOperator(Mesh *msh) {
		setMesh(msh);
	}

	MeshOperator::~MeshOperator() {
		
	}

	void MeshOperator::setMesh(Mesh *msh) {
		_mesh = msh;
	}

	Mesh *MeshOperator::mesh() {
		return _mesh;
	}

	std::vector<Face*> MeshOperator::facesFromIndices(std::vector<size_t> faces) {
		std::vector<Face*> fl;

		for (auto i : faces) {
			fl.push_back(mesh()->faces()[i]);
		}

		return fl;
	}

	std::vector<Edge*> MeshOperator::edgesFromIndices(std::vector<size_t> edges) {
		std::vector<Edge*> el;

		for (auto i : edges) {
			el.push_back(mesh()->edges()[i]);
		}

		return el;
	}

	// Calculate smooth normals
	void MeshOperator::calcSmoothNormals(std::string propertyName) {
		MeshQuery ms;
		if (!mesh()->hasProperty(propertyName, ds::PropertyType::VERTEX)) return;

		#pragma omp parallel default(shared)
		{
			#pragma omp for schedule(static)
			for (long long int inx = 0; inx < mesh()->vertices().size(); inx++) {
				auto v = mesh()->vertices()[inx];
				glm::vec3 normal_sum(0);

				auto fls = ms.vertexIncidentFaces(v);

				for (auto f : fls) {

					glm::vec3 u(f->vertices()[1]->position() - f->vertices()[0]->position());
					glm::vec3 v(f->vertices()[2]->position() - f->vertices()[0]->position());

					normal_sum += glm::cross(u, v);
				}

				glm::vec3 normi = glm::normalize(normal_sum);

				v->setProperty(propertyName, normi);
			}
		}
	}

	// Calculate face normals
	void MeshOperator::calcFaceNormals(std::string propertyName) {
		if (!mesh()->hasProperty(propertyName, ds::PropertyType::FACE)) return;

		#pragma omp parallel default(shared)
		{
			#pragma omp for schedule(static)
			for (long long inx = 0; inx < mesh()->faces().size(); inx++) {
				auto f = mesh()->faces()[inx];
				glm::vec3 u(f->vertices()[1]->position() - f->vertices()[0]->position());
				glm::vec3 v(f->vertices()[2]->position() - f->vertices()[0]->position());

				glm::vec3 normi = glm::normalize(glm::cross(u, v));

				f->setProperty(propertyName, normi);
			}
		}
	}

	// Delete Vertices
	void MeshOperator::deleteVertices(std::vector<size_t> verts) {	

		if (!verts.empty()) {
			std::vector<Vert*> temp;

			for (auto i : verts) temp.push_back(mesh()->vertices()[i]);
			for (auto v : temp) mesh()->removeVertex(v);

			mesh()->reindexVertices();
			mesh()->reindexFaces();
			mesh()->reindexEdges();
		}
	}

	// Delete Edges
	void MeshOperator::deleteEdges(std::vector<size_t> edges) {

		if (!edges.empty()) {
			std::vector<Edge*> temp;

			for (auto i : edges) temp.push_back(mesh()->edges()[i]);
			for (auto e : temp) mesh()->removeEdge(e);

			mesh()->reindexVertices();
			mesh()->reindexFaces();
			mesh()->reindexEdges();
		}
	}

	// Delete Faces
	void MeshOperator::deleteFaces(std::vector<size_t> faces) {

		if (!faces.empty()) {
			std::vector<Face*> temp;

			for (auto i : faces) temp.push_back(mesh()->faces()[i]);
			for (auto f : temp) mesh()->removeFace(f);

			mesh()->reindexVertices();
			mesh()->reindexFaces();
			mesh()->reindexEdges();
		}
	}

	// Extrude Faces
	std::vector<Face*> MeshOperator::extrudeFaces(std::vector<size_t> faces) {
		std::vector<Face*> output;

		// Debug counters
		int debug_vCounter = 0;
		int debug_eCounter = 0;

		if (!faces.empty()) {
			MeshQuery mq;
	
			// Temp buffers
			std::unordered_map<size_t, Vert*> oldNewVert; 
			std::vector<Edge*> newEdges;
			
			// Indices to face pointers
			auto fs = facesFromIndices(faces);

			// Main loop, for each face
			for (auto f : fs) {

				// Resolve orientation of andjacent face edges  
				std::vector<util::ProxyEdge> orientedEdges = util::sortFaceEdges(f);

				// Copy exiting face, first step of extrusion 
				EdgeList newFaceEdges;
				std::vector<Vert*> newFaceVerts;
				for (auto pe : orientedEdges) {

					// Find existing edges and verts created during extrusion
					// If the don't exist yet create them and store a reference for
					// future faces.
					Vert *vert00, *vert01;
					Edge *edge00;

					auto ifaces = mq.edgeIncidentFaces(pe.reference());

					int relatedFaces = 0;

					for (auto ifce : ifaces) {
						for (auto f : fs) {
							if (f->getUID() == ifce->getUID()) relatedFaces++;
						}
					}

					auto iter00 = oldNewVert.find(pe.a()->getUID());
					if (iter00 == oldNewVert.end()) {
						auto nvert = mesh()->addVertex(pe.a()->position());
						std::pair<size_t, Vert*> p(pe.a()->getUID(), nvert);
						oldNewVert.insert(p);

						vert00 = nvert;

						debug_vCounter++;
					}
					else {
						vert00 = iter00->second;
					}

					iter00 = oldNewVert.find(pe.b()->getUID());
					if (iter00 == oldNewVert.end()) {
						auto nvert = mesh()->addVertex(pe.b()->position());
						std::pair<size_t, Vert*> p(pe.b()->getUID(), nvert);
						oldNewVert.insert(p);

						vert01 = nvert;

						debug_vCounter++;
					}
					else {
						vert01 = iter00->second;
					}

					int index = 0;
					auto rcpair = util::hasEdge(vert00, vert01, newEdges, index);

					if (!rcpair.first) {
						auto nedge = mesh()->addEdge(vert00, vert01);

						std::pair<Edge*, Edge*> p(nedge, pe.reference());	
						newEdges.push_back(nedge);

						edge00 = nedge;

						debug_eCounter++;
					}
					else {
						edge00 = newEdges[index];
					}

					newFaceEdges.push_back(edge00);
					newFaceVerts.push_back(vert00);
					
					// Create Side faces
					// If this edge is a boundary edge in the extrusion, 
					// we will want to create side faces here.
					if (relatedFaces == 1) {
					
						Edge *edgeA;
						Edge *edgeB;

						// Check if edges for side face exist yet, otherwise create them
						rcpair = util::hasEdge(vert00, pe.a(), newEdges, index);
						if (!rcpair.first) {
							auto nedge = mesh()->addEdge(vert00, pe.a());

							std::pair<Edge*, Edge*> p(nedge, pe.reference());
							newEdges.push_back(nedge);

							edgeA = nedge;

							debug_eCounter++;
						}
						else {
							edgeA = newEdges[index];
						}

						rcpair = util::hasEdge(vert01, pe.b(), newEdges, index);
						if (!rcpair.first) {
							auto nedge = mesh()->addEdge(vert01, pe.b());

							std::pair<Edge*, Edge*> p(nedge, pe.reference());
							newEdges.push_back(nedge);

							edgeB = nedge;

							debug_eCounter++;
						}
						else {
							edgeB = newEdges[index];
						}

						// Colect edges and vertices
						// Make sure to use proxy edge verts and new verts,
						// to have consistent (clockwise) orientation for new
						// side face;
						ds::EdgeList sideel;
						std::vector<Vert*> sidevl;

						sideel.push_back(pe.reference());
						sideel.push_back(edgeB);
						sideel.push_back(edge00);
						sideel.push_back(edgeA);

						sidevl.push_back(pe.a());
						sidevl.push_back(pe.b());
						sidevl.push_back(edge00->v01());
						sidevl.push_back(edge00->v00());

						// Finally create side face
						mesh()->addFace(sideel, sidevl);
					}
				}

				// Save new face copy for output
				output.push_back(mesh()->addFace(newFaceEdges, newFaceVerts)); 
			}

			// Delete exisinting selected faces
			deleteFaces(faces);
		}

		// Don't forget to reindex everything for fast and logical access
		mesh()->reindexVertices();
		mesh()->reindexFaces();
		mesh()->reindexEdges();

		//Print debug  info
		//printf("New Vertices: %i\n", debug_vCounter);
		//printf("New Edges: %i\n", debug_eCounter);

		return output;
	}

	// Triangulate Mesh
	std::vector<Face*> MeshOperator::triangulateFace(std::vector<size_t> faces) {
		std::vector<Face*> output;
		MeshBuilder meshBuilder;
		
		// Indices to face pointers
		auto fs = facesFromIndices(faces);

		for (auto f: fs) {

			if (f->verticesCount() == 3) continue;

			std::vector<glm::vec3> newVerts;
			std::vector<Edge*> existingEdges;
			std::unordered_map<size_t, Vert*> originalVerts;

			ds::Tesselator tess;
			tess.init();
			tess.newPoly(f->verticesCount());
			for (int j = 0; j < f->verticesCount(); j++) {
				tess.addVertex(j, 
					f->vertices()[j]->position().x, 
					f->vertices()[j]->position().y, 
					f->vertices()[j]->position().z);
			}

			tess.commitPoly();
			tess.tesselate();

			for (int i = 0; i < f->verticesCount(); i++) {
				size_t current = 3 * i;
				glm::vec3 v(tess.vertValue(current), tess.vertValue(current + 1), tess.vertValue(current + 2));
				newVerts.push_back(v);
			}

			for (size_t i = 0; i < newVerts.size(); i++) {
				for (int j = 0; j < f->verticesCount(); j++) {
					if (newVerts[i] == f->vertices()[j]->position()) {

						auto vv = f->vertices()[j];
						std::pair<size_t, Vert*> p(i, vv);
						originalVerts.insert(p);
					}
				}
			}

			for (auto e : f->adjacentEdges()) {
				existingEdges.push_back(e);
			}
			
			for (int i = 0; i < tess.numberOfTriangles(); i++) {
				std::vector<size_t> indices;

				size_t current = 3 * i;
				int inx00 = tess.faceIndex(current);
				int inx01 = tess.faceIndex(current + 1);
				int inx02 = tess.faceIndex(current + 2);

				auto a = originalVerts.find(inx00)->second;
				auto b = originalVerts.find(inx01)->second;
				auto c = originalVerts.find(inx02)->second;

				Vert *a00 = nullptr;
				Vert *b00 = nullptr;
				Vert *c00 = nullptr;

				Edge *e00 = nullptr;
				Edge *e01 = nullptr;
				Edge *e02 = nullptr;

				for (auto e : existingEdges) {

					auto rc00 = util::edgeIdentical(a, b, e);
					auto rc01 = util::edgeIdentical(b, c, e);
					auto rc02 = util::edgeIdentical(c, a, e);

					if (rc00.first) {
						e00 = e;
					}

					if (rc01.first) {
						e01 = e;
					}

					if (rc02.first) {
						e02 = e;

					}
				}

				if (e00 == nullptr) {
					auto nedge = mesh()->addEdge(a, b);
					e00 = nedge;
					existingEdges.push_back(nedge);
				}

				if (e01 == nullptr) {
					auto nedge = mesh()->addEdge(b, c);
					e01 = nedge;
					existingEdges.push_back(nedge);
				}

				if (e02 == nullptr) {
					auto nedge = mesh()->addEdge(c, a);
					e02 = nedge;
					existingEdges.push_back(nedge);
				}
				
				EdgeList el;
				el.push_back(e00);
				el.push_back(e01);
				el.push_back(e02);

				std::vector<Vert*> vts;
				vts.push_back(a);
				vts.push_back(b);
				vts.push_back(c);

				auto newFace = mesh()->addFace(el, vts);
				output.push_back(newFace);
			}

			mesh()->removeFace(f);

			tess.destroy();
		}

		mesh()->reindexVertices();
		mesh()->reindexFaces();
		mesh()->reindexEdges();

		return output;
	}

	// Bevel Edges
	std::vector<Edge*> MeshOperator::bevelEdges(std::vector<size_t> edges, float offset) {
		MeshQuery mq;
		std::vector<Edge*> output;

		if (!edges.empty()) {
			std::unordered_map<size_t, Vert*> points;

			auto es = edgesFromIndices(edges);
			std::vector<Edge*> splitEdges;
			std::unordered_map<size_t, Edge*> oldNewEdge;
			std::unordered_map<size_t, Edge*> newOldEdge;
			std::unordered_map<size_t, Edge*> mainEdges;
			std::unordered_map<size_t, Vert*> newPoints;


			for (auto e : es) {
				auto ifs = mq.edgeIncidentFaces(e);
				if (ifs.size() != 2) return output;
			}

			for (auto e : es) {

				auto ifs = mq.edgeIncidentFaces(e);

				Vert *v00a = nullptr;
				Vert *v01a = nullptr;
				Vert *v00b = nullptr;
				Vert *v01b = nullptr;
				
				Edge *a = nullptr;
				Edge *b = nullptr;

				int cnt = 0;
				for (auto f : ifs) {
					std::vector<util::ProxyEdge> newEdges;
					for (auto fe :util::sortFaceEdges(f)) {
						auto ed_check = oldNewEdge.find(fe.reference()->getUID());
						std::vector<Edge*> mainEdgeRefs;

						/*if (fe.reference()->getUID() == e->getUID() && ed_check != oldNewEdge.end()) {
							printf("Bevel Edge Already Exists...\n");
						}
						*/
						if (fe.reference()->getUID() != e->getUID()) {
							bool edgeExists = false;
							bool isNewMainEdge = false;
							Edge *tempe00 = nullptr;	

							if (ed_check != oldNewEdge.end()) edgeExists = true;
							for (auto meg : newOldEdge) {
								if (meg.second->getUID() == fe.reference()->getUID()) {
									mainEdgeRefs.push_back(mainEdges.find(meg.first)->second);
								}
							}

							if (!mainEdgeRefs.empty()) {
								printf("Edge references old edge...\n");
							}
							else if (edgeExists) {
								printf("Already Exists...\n");
								auto npt00 = newPoints.find(ed_check->second->v00()->getUID());
								auto npt01 = newPoints.find(ed_check->second->v01()->getUID());

								util::ProxyEdge pe;
								pe.setReference(ed_check->second);

								if (e->v00()->getUID() == fe.a()->getUID()) {
									if (cnt == 0) {
										//v00a;		
										if (npt00 != newPoints.end()) {
											v00a = npt00->second;
											pe.setA(v00a);
											pe.setB(ed_check->second->v01());
										}
										else if (npt01 != newPoints.end()) {
											v00a = npt01->second;
											pe.setA(v00a);
											pe.setB(ed_check->second->v00());
										}
									}
									else {
										//v00b;
										if (npt00 != newPoints.end()) {
											v00b = npt00->second;
											pe.setA(v00b);
											pe.setB(ed_check->second->v01());
										}
										else if (npt01 != newPoints.end()) {
											v00b = npt01->second;
											pe.setA(v00b);
											pe.setB(ed_check->second->v00());
										}
									}

									newEdges.push_back(pe);
								}
								else if (e->v00()->getUID() == fe.b()->getUID()) {
									if (cnt == 0) {
										//v00a;
										if (npt00 != newPoints.end()) {
											v00a = npt00->second;
											pe.setA(ed_check->second->v01());
											pe.setB(v00a);
										}
										else if (npt01 != newPoints.end()) {
											v00a = npt01->second;
											pe.setA(ed_check->second->v00());
											pe.setB(v00a);
										}
									}
									else {
										//v00b;
										if (npt00 != newPoints.end()) {
											v00b = npt00->second;
											pe.setA(ed_check->second->v01());
											pe.setB(v00b);
										}
										else if (npt01 != newPoints.end()) {
											v00b = npt01->second;
											pe.setA(ed_check->second->v00());
											pe.setB(v00b);
										}
									}

									newEdges.push_back(pe);
								}
								else if (e->v01()->getUID() == fe.a()->getUID()) {
									if (cnt == 0) {
										//v01a;
										if (npt00 != newPoints.end()) {
											v01a = npt00->second;
											pe.setA(v01a);
											pe.setB(ed_check->second->v01());
										}
										else if (npt01 != newPoints.end()) {
											v01a = npt01->second;
											pe.setA(v01a);
											pe.setB(ed_check->second->v00());
										}
									}
									else {
										//v01b;
										if (npt00 != newPoints.end()) {
											v01b = npt00->second;
											pe.setA(v01b);
											pe.setB(ed_check->second->v01());
										}
										else if (npt01 != newPoints.end()) {
											v01b = npt01->second;
											pe.setA(v01b);
											pe.setB(ed_check->second->v00());
										}
									}

									newEdges.push_back(pe);
								}
								else if (e->v01()->getUID() == fe.b()->getUID()) {
									if (cnt == 0) {
										//v01a;
										if (npt00 != newPoints.end()) {
											v01a = npt00->second;
											pe.setA(ed_check->second->v01());
											pe.setB(v01a);
										}
										else if (npt01 != newPoints.end()) {
											v01a = npt01->second;
											pe.setA(ed_check->second->v00());
											pe.setB(v01a);
										}
									}
									else {
										//v01b;
										if (npt00 != newPoints.end()) {
											v01b = npt00->second;
											pe.setA(ed_check->second->v01());
											pe.setB(v01b);
										}
										else if (npt01 != newPoints.end()) {
											v01b = npt01->second;
											pe.setA(ed_check->second->v00());
											pe.setB(v01b);
										}
									}

									newEdges.push_back(pe);
								}
								else {

									util::ProxyEdge _pe;
									_pe.setA(ed_check->second->v00());
									_pe.setB(ed_check->second->v01());
									_pe.setReference(ed_check->second);
									newEdges.push_back(_pe);
								}
								
							}
							else{

								if (e->v00()->getUID() == fe.a()->getUID()) {
									if (cnt == 0) {
										if(v00a == nullptr) v00a = mesh()->addVertex(e->v00()->position());
										tempe00 = mesh()->addEdge(v00a, fe.b());

										std::pair<size_t, Vert*> p(v00a->getUID(), v00a);
										newPoints.insert(p);
									}
									else {
										if (v00b == nullptr) v00b = mesh()->addVertex(e->v00()->position());
										tempe00 = mesh()->addEdge(v00b, fe.b());

										std::pair<size_t, Vert*> p(v00b->getUID(), v00b);
										newPoints.insert(p);
									}
								}
								else if (e->v00()->getUID() == fe.b()->getUID()) {
									if (cnt == 0) {
										if (v00a == nullptr) v00a = mesh()->addVertex(e->v00()->position());
										tempe00 = mesh()->addEdge(fe.a(), v00a);

										std::pair<size_t, Vert*> p(v00a->getUID(), v00a);
										newPoints.insert(p);
									}
									else {
										if (v00b == nullptr) v00b = mesh()->addVertex(e->v00()->position());
										tempe00 = mesh()->addEdge(fe.a(), v00b);

										std::pair<size_t, Vert*> p(v00b->getUID(), v00b);
										newPoints.insert(p);
									}
								}
								else if (e->v01()->getUID() == fe.a()->getUID()) {
									if (cnt == 0) {
										if (v01a == nullptr) v01a = mesh()->addVertex(e->v01()->position());
										tempe00 = mesh()->addEdge(v01a, fe.b());

										std::pair<size_t, Vert*> p(v01a->getUID(), v01a);
										newPoints.insert(p);
									}
									else {
										if (v01b == nullptr) v01b = mesh()->addVertex(e->v01()->position());
										tempe00 = mesh()->addEdge(v01b, fe.b());

										std::pair<size_t, Vert*> p(v01b->getUID(), v01b);
										newPoints.insert(p);
									}
								}
								else if (e->v01()->getUID() == fe.b()->getUID()) {
									if (cnt == 0) {
										if (v01a == nullptr) v01a = mesh()->addVertex(e->v01()->position());
										tempe00 = mesh()->addEdge(fe.a(), v01a);

										std::pair<size_t, Vert*> p(v01a->getUID(), v01a);
										newPoints.insert(p);
									}
									else {
										if (v01b == nullptr) v01b = mesh()->addVertex(e->v01()->position());
										tempe00 = mesh()->addEdge(fe.a(), v01b);

										std::pair<size_t, Vert*> p(v01b->getUID(), v01b);
										newPoints.insert(p);
									}
								}

								if (tempe00 != nullptr) {
									util::ProxyEdge pe;
									pe.setA(tempe00->v00());
									pe.setB(tempe00->v01());
									pe.setReference(tempe00);

									newEdges.push_back(pe);

									std::pair<size_t, Edge*> p(fe.reference()->getUID(), tempe00);
									oldNewEdge.insert(p);
									output.push_back(tempe00);
								}
								else {
									newEdges.push_back(fe);
								}
							}
						}
						else {				
							newEdges.push_back(fe);
						}
					}

					if (v00a != nullptr && v01a != nullptr) {
						a = mesh()->addEdge(v00a, v01a);
						std::pair<size_t, Edge*> p00(a->getUID(), e);
						std::pair<size_t, Edge*> p01(a->getUID(), a);
						newOldEdge.insert(p00);
						mainEdges.insert(p01);

						v00a = nullptr;
						v01a = nullptr;
					}

					if (v00b != nullptr && v01b != nullptr) {
						b = mesh()->addEdge(v00b, v01b);
						std::pair<size_t, Edge*> p00(b->getUID(), e);
						std::pair<size_t, Edge*> p01(b->getUID(), b);
						newOldEdge.insert(p00);
						mainEdges.insert(p01);

						v00b = nullptr;
						v01b = nullptr;
					}

					EdgeList nfaceEdges;
					std::vector<Vert*> nfaceVerts;

					// Move Points data
					Face *nface = nullptr;
					Vert *dirA  = nullptr;
					Vert *dirB  = nullptr;

					// Create face
					if (newEdges.size() > 4 || newEdges.size() < 4) {
						printf("Bevel: We have a problem!!\n");
					}
					for (auto _e : newEdges) {
						
						if ( _e.reference()->getUID() == e->getUID()) {
							if (cnt == 0) {
								nfaceEdges.push_back(a);
								if (_e.a()->getUID() == e->v00()->getUID()) {	
									nfaceVerts.push_back(a->v00());
									dirA = a->v00();
									dirB = a->v01();
								}
								else {
									nfaceVerts.push_back(a->v01());
									dirA = a->v01();
									dirB = a->v00();
								}
							}
							else {
								nfaceEdges.push_back(b);
								if (_e.a()->getUID() == e->v00()->getUID()) {
									nfaceVerts.push_back(b->v00());
									dirA = a->v00();
									dirB = a->v01();
								}
								else {
									nfaceVerts.push_back(b->v01());
									dirA = a->v01();
									dirB = a->v00();
								}
							}
						}
						else {
							nfaceEdges.push_back(_e.reference());
							nfaceVerts.push_back(_e.a());
						}
					}
					nface = mesh()->addFace(nfaceEdges, nfaceVerts);

					// Move Points
					glm::vec3 u(f->vertices()[1]->position() - f->vertices()[0]->position());
					glm::vec3 v(f->vertices()[2]->position() - f->vertices()[0]->position());

					glm::vec3 normi = glm::normalize(glm::cross(u, v));
					glm::vec3 dispDir = glm::cross(normi, (dirB->position() - dirA->position()));

					if (cnt == 0) {
						a->v00()->position() = a->v00()->position() + (offset * dispDir);
						a->v01()->position() = a->v01()->position() + (offset * dispDir);
					}
					else {
						b->v00()->position() = b->v00()->position() + (offset * dispDir);
						b->v01()->position() = b->v01()->position() + (offset * dispDir);
					}

					cnt++;

				}

				for (auto vt: mq.edgeAdjacentVertices(e)) {

					if (util::isEndPoint(vt, es)) {

						for (auto e_fs : mq.vertexIncidentFaces(vt)) {

							if (e_fs->getUID() != ifs.front()->getUID() &&
								e_fs->getUID() != ifs.back()->getUID()) {

								glm::vec3 splitDir00(0);
								glm::vec3 splitDir01(0);
								ds::Vert *split00 = nullptr;
								ds::Vert *split01 = nullptr;
								ds::Vert *other00 = nullptr;
								ds::Vert *other01 = nullptr;
								ds::Edge *oldEdge00 = nullptr;
								ds::Edge *oldEdge01 = nullptr;
								bool edge00_exists = false;
								bool edge01_exists = false;

								for (auto e_fs_e : util::sortFaceEdges(e_fs)) {
									auto ed_check = oldNewEdge.find(e_fs_e.reference()->getUID());

									if (ed_check != oldNewEdge.end()) {
										if (vt->getUID() == e_fs_e.a()->getUID()) {
											split00 = ed_check->second->v01();
											other00 = ed_check->second->v00();

											edge00_exists = true;
										}
										else if (vt->getUID() == e_fs_e.b()->getUID()) {
											split01 = ed_check->second->v00();
											other01 = ed_check->second->v01();

											edge01_exists = true;
										}
									}
									else {
										if (vt->getUID() == e_fs_e.a()->getUID()) {
											split00 = mesh()->addVertex(vt->position());
											other00 = e_fs_e.b();

											splitDir00 = glm::normalize(e_fs_e.a()->position() - e_fs_e.b()->position());

											oldEdge00 = e_fs_e.reference();
										}
										else if (vt->getUID() == e_fs_e.b()->getUID()) {
											split01 = mesh()->addVertex(vt->position());
											other01 = e_fs_e.a();

											splitDir01 = glm::normalize(e_fs_e.a()->position() - e_fs_e.b()->position());

											oldEdge01 = e_fs_e.reference();
										}
									}
								}

								if (split00 != nullptr &&
									split01 != nullptr &&
									other00 != nullptr &&
									other01 != nullptr) {

									Edge *splitEdge = mesh()->addEdge(split00, split01);
									splitEdges.push_back(splitEdge);

									output.push_back(splitEdge);
									if (!edge00_exists) {
										Edge *edge00 = mesh()->addEdge(split00, other00);

										std::pair<size_t, Edge*> p(oldEdge00->getUID(), edge00);
										oldNewEdge.insert(p);

										split00->position() = split00->position() + (-(offset*1.f) * splitDir00);
										//printf("New Edge Created 00...\n");
									}

									if (!edge01_exists) {
										Edge *edge01 = mesh()->addEdge(other01, split01);

										std::pair<size_t, Edge*> p(oldEdge01->getUID(), edge01);
										oldNewEdge.insert(p);

										split01->position() = split01->position() + ((offset*1.f) * splitDir01);
										//printf("New Edge Created 01...\n");
									}
									/*Vert *a = mesh()->addVertex(vt->position());
									Vert *b = mesh()->addVertex(vt->position());*/
								}
								else {
									printf("Bevel: Failed end point split\n");
								}
							}
						}
					}
				}

				
			}

			for (auto e : es) {
				for (auto tf : mq.edgeIncidentFaces(e)) mesh()->removeFace(tf);
			}
		}

		// Don't forget to reindex everything for fast and logical access
		mesh()->reindexVertices();
		mesh()->reindexFaces();
		mesh()->reindexEdges();

		return output;
	}

	std::vector<Edge*> MeshOperator::bevelEdges00(std::vector<size_t> edges, float offset) {
		MeshQuery mq;
		std::vector<Edge*> output;

		if (!edges.empty()) {
			std::unordered_map<size_t, Vert*> points;
			std::vector<BeveledEdge*> beveledEdges;

			auto es = edgesFromIndices(edges);

			for (auto e : es) {
				auto ifs = mq.edgeIncidentFaces(e);
				if (ifs.size() != 2) return output;
			}

			// Split Edges
			for (auto e : es) {
				auto ifs = mq.edgeIncidentFaces(e);

				BeveledEdge *be = new BeveledEdge;
				be->originalEdge = e;
				be->originalAdjacentFace_a = ifs.front();
				be->originalAdjacentFace_b = ifs.back();
				be->v00_Midpoint = (!util::isEndPoint(e->v00(), es));
				be->v01_Midpoint = (!util::isEndPoint(e->v01(), es));
				be->v00a = nullptr;
				be->v01a = nullptr;
				be->v00b = nullptr;
				be->v01b = nullptr;
				
				int cnt = 0;
				for (auto f : ifs) {

					for (auto fe : util::sortFaceEdges(f)) {

						if (fe.reference()->getUID() != e->getUID()) {
							bool newEdgeDetected = false;

							if (e->v00()->getUID() == fe.a()->getUID()) {
								if (cnt == 0) {
									if (be->v00a == nullptr) {
										be->v00a = mesh()->addVertex(e->v00()->position());
										be->v00a_old = e->v00();

										util::ProxyEdge pe;
										pe.setA(be->v00a);
										pe.setB(fe.b());

										be->newFace_a.push_back(pe);
									}
								}
								else {
									if (be->v00b == nullptr) {
										be->v00b = mesh()->addVertex(e->v00()->position());
										be->v00b_old = e->v00();

										util::ProxyEdge pe;
										pe.setA(be->v00b);
										pe.setB(fe.b());

										be->newFace_b.push_back(pe);
									}
								}

								newEdgeDetected = true;
							}
							else if (e->v00()->getUID() == fe.b()->getUID()) {
								if (cnt == 0) {
									if (be->v00a == nullptr) {
										be->v00a = mesh()->addVertex(e->v00()->position());
										be->v00a_old = e->v00();

										util::ProxyEdge pe;
										pe.setA(fe.a());
										pe.setB(be->v00a);

										be->newFace_a.push_back(pe);
									}
									
								}
								else {
									if (be->v00b == nullptr) {
										be->v00b = mesh()->addVertex(e->v00()->position());
										be->v00b_old = e->v00();

										util::ProxyEdge pe;
										pe.setA(fe.a());
										pe.setB(be->v00b);

										be->newFace_b.push_back(pe);
									}
									
								}

								newEdgeDetected = true;
							}
							else if (e->v01()->getUID() == fe.a()->getUID()) {
								if (cnt == 0) {
									if (be->v01a == nullptr) {
										be->v01a = mesh()->addVertex(e->v01()->position());
										be->v01a_old = e->v01();

										util::ProxyEdge pe;
										pe.setA(be->v01a);
										pe.setB(fe.b());								

										be->newFace_a.push_back(pe);
									}
									
								}
								else {
									if (be->v01b == nullptr) {
										be->v01b = mesh()->addVertex(e->v01()->position());
										be->v01b_old = e->v01();

										util::ProxyEdge pe;
										pe.setA(be->v01b);
										pe.setB(fe.b());

										be->newFace_b.push_back(pe);
									}
								
								}

								newEdgeDetected = true;
							}
							else if (e->v01()->getUID() == fe.b()->getUID()) {
								if (cnt == 0) {
									if (be->v01a == nullptr) {
										be->v01a = mesh()->addVertex(e->v01()->position());
										be->v01a_old = e->v01();

										util::ProxyEdge pe;
										pe.setA(fe.a());
										pe.setB(be->v01a);
										
										be->newFace_a.push_back(pe);
									}
									
								}
								else {
									if (be->v01b == nullptr) {
										be->v01b = mesh()->addVertex(e->v01()->position());
										be->v01b_old = e->v01();

										util::ProxyEdge pe;
										pe.setA(fe.a());
										pe.setB(be->v01b);

										be->newFace_b.push_back(pe);
									}
									
								}

								newEdgeDetected = true;
							}

							if (!newEdgeDetected) {
								if (cnt == 0) {
									be->newFace_a.push_back(fe);
								}
								else {
									be->newFace_b.push_back(fe);
								}
							}
						}
						else {
							if (cnt == 0) {
								be->newFace_a.push_back(fe);
							}
							else {
								be->newFace_b.push_back(fe);
							}
						}
					}

					if (cnt == 0) {
						if (be->v00a != nullptr && be->v01a != nullptr) printf("Edge Split A Success\n\n");
						for (auto &_e : be->newFace_a) {

							if (_e.reference() != nullptr) {
								if (_e.reference()->getUID() == e->getUID()) {

									if (_e.a()->getUID() == e->v00()->getUID()) {
										_e.setA(be->v00a);
										_e.setB(be->v01a);
										_e.setReference(nullptr);
									}
									else {
										_e.setA(be->v01a);
										_e.setB(be->v00a);
										_e.setReference(nullptr);
									}
								}
							}
						}
					}
					else {
						if (be->v00b != nullptr && be->v01b != nullptr) printf("Edge Split B Success\n\n");
						for (auto &_e : be->newFace_b) {

							if (_e.reference() != nullptr) {
								if (_e.reference()->getUID() == e->getUID()) {

									if (_e.a()->getUID() == e->v00()->getUID()) {
										_e.setA(be->v00b);
										_e.setB(be->v01b);
										_e.setReference(nullptr);
									}
									else {
										_e.setA(be->v01b);
										_e.setB(be->v00b);
										_e.setReference(nullptr);
									}
								}
							}
						}
					}

					cnt++;
				}

				//printf("New Face A Edge Count %i\n", be.newFace_a.size());
				
				//printf("New Face B Edge Count %i\n", be.newFace_b.size());
				beveledEdges.push_back(be);
			}

			// Find faces to merge
			std::unordered_map<size_t, BeveledTempFaceMerge> mergedFaces;
			std::unordered_map<size_t, Vert*> mergedPoint;
			std::unordered_map<size_t, Face*> faceVisited;
			for (auto &be00 : beveledEdges) {

				std::vector<Face*> ifs00;
				ifs00.push_back(be00->originalAdjacentFace_a);
				ifs00.push_back(be00->originalAdjacentFace_b);
				
				for (auto f00 : ifs00) {
					auto fv_check = faceVisited.find(f00->getUID());

					if (fv_check != faceVisited.end()) continue;
					else {
						std::pair<size_t, Face*> p(f00->getUID(), f00);
						faceVisited.insert(p);
					}

					bool mergeFace = false;
					BeveledTempFaceMerge btfm;
					std::vector<util::ProxyEdge> *pePtr00 = nullptr;

					btfm.originalUID = f00->getUID();
					for (auto sorted_e : util::sortFaceEdges(f00)) {
						btfm.newedges.push_back(util::ProxyEdge());
						btfm.refereceEdges.push_back(sorted_e);
						btfm.overideFlags.push_back(std::pair<bool,bool>(false,false));
					}

					if (be00->originalAdjacentFace_a->getUID() == f00->getUID()) {
						pePtr00 = &be00->newFace_a;
					}
					else if(be00->originalAdjacentFace_b->getUID() == f00->getUID()){
						pePtr00 = &be00->newFace_b;
					}
					else {
						continue;
					}

					int edgeCounter = 0;
					for (auto &ed00 : *pePtr00) {

						if (ed00.a()->getUID() != btfm.refereceEdges[edgeCounter].a()->getUID()) btfm.overideFlags[edgeCounter].first = true;
						if (ed00.b()->getUID() != btfm.refereceEdges[edgeCounter].b()->getUID()) btfm.overideFlags[edgeCounter].second = true;
						
						edgeCounter++;
					}

					for (auto &be01 : beveledEdges) {

						if (be00->originalEdge->getUID() != be01->originalEdge->getUID()) {
							std::vector<Face*> ifs01;
							ifs01.push_back(be01->originalAdjacentFace_a);
							ifs01.push_back(be01->originalAdjacentFace_b);
						
							std::vector<util::ProxyEdge> *pePtr01 = nullptr;

							for (auto *f01: ifs01) {
								
								if (f01->getUID() == f00->getUID()) {

									if (be01->originalAdjacentFace_a->getUID() == f01->getUID()) {
										pePtr01 = &be01->newFace_a;
									}
									else if(be01->originalAdjacentFace_b->getUID() == f01->getUID()){
										pePtr01 = &be01->newFace_b;
									}
									else {
										continue;
									}

									
									edgeCounter = 0;
									for (auto &ed01 : *pePtr01) {

										if (ed01.a()->getUID() != btfm.refereceEdges[edgeCounter].a()->getUID()) btfm.overideFlags[edgeCounter].first = true;
										if (ed01.b()->getUID() != btfm.refereceEdges[edgeCounter].b()->getUID()) btfm.overideFlags[edgeCounter].second = true;

										edgeCounter++;
									}

									mergeFace = true;
								//	
								//	for (auto &ed00 : *pePtr00) {
								//
								//		for (auto &ed01 : *pePtr01) {
								//			printf("A: %i, B: %i\n", ed01.a()->getUID(), ed01.b()->getUID());
								//			auto dist = glm::distance(ed00.a()->position(), ed01.a()->position());
								//			if (dist == 0) {
								//				if (ed00.a()->getUID() != ed01.a()->getUID()) {
								//					ed01.setA(ed00.a());
								//					printf("Merge00!!!\n");
								//					printf("Edge00 %i, Face %i;\n", be00->originalEdge->getUID(), f00->getUID());
								//					/*if (cnt00 == 0) printf("Side A\n");
								//					else printf("Side B\n\n");*/

								//					printf("Edge01 %i, Face %i;\n", be01->originalEdge->getUID(), f01->getUID());
								//					/*if (cnt01 == 0) printf("Side A\n");
								//					else printf("Side B\n\n");*/
								//				}
								//			}
								//		}
								//		printf("\n");
								//	}
								}

							}
						}

						//for()
						
					}
					
					if (mergeFace) {
						std::pair<size_t, BeveledTempFaceMerge> p(btfm.originalUID, btfm);
						mergedFaces.insert(p);
					}
				}
				
			}

			// Assemble Merged faces & create associated edges and verts
			for (auto &mf : mergedFaces) {
				int counter = 0;

				// Assemble, merge
				for (auto fp : mf.second.overideFlags) {
					if (fp.first) {
						auto vt = mesh()->addVertex(mf.second.refereceEdges[counter].a()->position());
						mf.second.newedges[counter].setA(vt);

						if (counter > 0) {
							mf.second.newedges[counter - 1].setB(vt);
						}

						
					}
					else {
						mf.second.newedges[counter].setA(mf.second.refereceEdges[counter].a());

						if (counter > 0) {
							mf.second.newedges[counter - 1].setB(mf.second.refereceEdges[counter].a());
						}
					}

					if (counter == (mf.second.overideFlags.size() - 1)) {
						mf.second.newedges[counter].setB(mf.second.newedges[0].a());
					}
				
					if (!fp.first && !fp.second) printf("Orginal Edge intact...\n");

					counter++;
				}

				// Create edges
				counter = 0;
				for (auto &fp : mf.second.newedges) {
					if (mf.second.overideFlags[counter].first || mf.second.overideFlags[counter].second) {
						auto ne = mesh()->addEdge(fp.a(), fp.b());
						fp.setReference(ne);
					}
					else {
						fp.setReference(mf.second.refereceEdges[counter].reference());
					}
					counter++;
				}

				
			}

			for (auto &be : beveledEdges) {
				for (auto &mf : mergedFaces) {

					if (be->originalAdjacentFace_a->getUID() == mf.second.originalUID) {
						printf("Merge face00...\n");

						for (int i = 0; i < be->newFace_a.size(); i++) {
							if (be->v00a->getUID() == be->newFace_a[i].a()->getUID()) {
								mesh()->removeVertex(be->v00a);
								be->v00a = mf.second.newedges[i].a();
							}

							if (be->v00a->getUID() == be->newFace_a[i].b()->getUID()) {
								mesh()->removeVertex(be->v00a);
								be->v00a = mf.second.newedges[i].b();
							}

							if (be->v01a->getUID() == be->newFace_a[i].a()->getUID()) {
								mesh()->removeVertex(be->v01a);
								be->v01a = mf.second.newedges[i].a();
							}

							if (be->v01a->getUID() == be->newFace_a[i].b()->getUID()) {
								mesh()->removeVertex(be->v01a);
								be->v01a = mf.second.newedges[i].b();
							}

							be->newFace_a[i].setA(mf.second.newedges[i].a());
							be->newFace_a[i].setB(mf.second.newedges[i].b());
							be->newFace_a[i].setReference(mf.second.newedges[i].reference());
						}
					}

					if (be->originalAdjacentFace_b->getUID() == mf.second.originalUID) {

						for (int i = 0; i < be->newFace_b.size(); i++) {
							if (be->v00b->getUID() == be->newFace_b[i].a()->getUID()) {
								mesh()->removeVertex(be->v00b);
								be->v00b = mf.second.newedges[i].a();
							}

							if (be->v00b->getUID() == be->newFace_b[i].b()->getUID()) {
								mesh()->removeVertex(be->v00b);
								be->v00b = mf.second.newedges[i].b();
							}

							if (be->v01b->getUID() == be->newFace_b[i].a()->getUID()) {
								mesh()->removeVertex(be->v01b);
								be->v01b = mf.second.newedges[i].a();
							}

							if (be->v01b->getUID() == be->newFace_b[i].b()->getUID()) {
								mesh()->removeVertex(be->v01b);
								be->v01b = mf.second.newedges[i].b();
							}

							be->newFace_b[i].setA(mf.second.newedges[i].a());
							be->newFace_b[i].setB(mf.second.newedges[i].b());
							be->newFace_b[i].setReference(mf.second.newedges[i].reference());
						}
					}
				}
			}
			// Make Edges
			for (auto &be : beveledEdges) {

				for (auto &edc : be->newFace_a) {
					if (edc.reference() == nullptr) {
						int index = 0;
						auto ed_check = false; // util::hasEdge(edc.a(), edc.b(), output, index);
						
						if (!ed_check) {
							auto _ref = mesh()->addEdge(edc.a(), edc.b());
							edc.setReference(_ref);

							output.push_back(_ref);
						}
					}
				}

				for (auto &edc : be->newFace_b) {
					if (edc.reference() == nullptr) {
						int index = 0;
						auto ed_check = false; // util::hasEdge(edc.a(), edc.b(), output, index);

						if (!ed_check) {
							auto _ref = mesh()->addEdge(edc.a(), edc.b());
							edc.setReference(_ref);

							output.push_back(_ref);
						}
					}
				}
			}

			//// Make Faces
			//for (auto be : beveledEdges) {
			//	EdgeList eds00;
			//	std::vector<Vert*> verts00;
			//	EdgeList eds01;
			//	std::vector<Vert*> verts01;

			//	for (auto _es: be->newFace_a) {
			//		eds00.push_back(_es.reference());
			//		verts00.push_back(_es.a());

			//		printf("Face A = A: %i, B: %i\n", _es.a()->getUID(), _es.b()->getUID());
			//	}
			//	printf("\n");
			//	mesh()->addFace(eds00, verts00);

			//	for (auto _es : be->newFace_b) {
			//		eds01.push_back(_es.reference());
			//		verts01.push_back(_es.a());

			//		printf("Face B = A: %i, B: %i\n", _es.a()->getUID(), _es.b()->getUID());
			//	}
			//	printf("\n");
			//	mesh()->addFace(eds01, verts01);
			//}
		}

		// Don't forget to reindex everything for fast and logical access
		mesh()->reindexVertices();
		mesh()->reindexFaces();
		mesh()->reindexEdges();

		return output;
	}

	// Bevel Edges
	std::vector<Edge*> MeshOperator::bevelEdges01(std::vector<size_t> edges, float offset) {	
		MeshQuery mq;
		std::vector<Edge*> output;
		std::unordered_map<size_t, Edge*> edgeMap;
		std::unordered_map<size_t, Edge*> newOldBevelEdge;
		std::unordered_map<size_t, Edge*> replacedBevelEdge;
		std::unordered_map<size_t, Vert*> oldNewMidPoint;

		if (!edges.empty()) {
			std::unordered_map<size_t, Vert*> points;
			std::vector<BeveledEdge*> beveledEdges;
			std::vector<Face*> garbageFaces;
			std::vector<Edge*> garbageEdges;
			std::vector<Vert*> garbageVerts;
			std::unordered_map<size_t, BevelTempFace> newFaceReference;

			auto es = edgesFromIndices(edges);

			for (auto e : es) {
				auto ifs = mq.edgeIncidentFaces(e);
				if (ifs.size() != 2) return output;

				std::pair<size_t, Edge*> pair(e->getUID(), e);
				edgeMap.insert(pair);
			}

			int cnt = 0;
			std::unordered_map<size_t, Vert*> visited_midpoints;
			for (auto e : es) {
				std::vector<std::pair<Vert*, Vert*>> midPoints;
				//Vert *other = nullptr;
				bool hasMidpoint = false;
				
				if (!util::isEndPoint(e->v00(), es)) {
					std::pair<Vert*, Vert*> p(e->v00(), e->v01());
					midPoints.push_back(p);
					hasMidpoint = true;
				}
				
				if (!util::isEndPoint(e->v01(), es)) {
					std::pair<Vert*, Vert*> p(e->v01(), e->v00());
					midPoints.push_back(p);
					hasMidpoint = true;
				}

				if (hasMidpoint) {

					for (auto mid_pair : midPoints) {
						auto midPoint = mid_pair.first;
						auto other = mid_pair.second;
						
						auto itr = visited_midpoints.find(midPoint->getUID());

						if (itr == visited_midpoints.end()) {
							printf("Midpoint...\n");
							auto edgeLoop = util::pointAdjacentEdgesCCW(e, midPoint, other);

							printf("midpoint: %i other: %i\n\n", midPoint->getUID(), other->getUID());

							if (edgeLoop.size() > 1) {

								bool newpass = true;
								std::vector<util::ProxyEdge> mid;
								for (int i = 0; i < edgeLoop.size(); i++) {

									if (i > 0 && edgeLoop[i].reference()->getUID() == e->getUID()) break;

									mid.push_back(edgeLoop[i]);
									auto itr = edgeMap.find(edgeLoop[i].reference()->getUID());

									if (itr != edgeMap.end()) {

										if (mid.size() >= 2) {
											//std::unordered_map<size_t, Edge*> oldNewEdge;
											auto centralPoint = mesh()->addVertex(midPoint->position());
											oldNewMidPoint[midPoint->getUID()] = centralPoint;

											for (int j = 0; j < mid.size(); j++) {

												bool skip = false;
												Edge *_nedge = nullptr;

												auto on_itr = newOldBevelEdge.find(mid[j].reference()->getUID());

												if (on_itr != newOldBevelEdge.end()) {
													
													bool process = false;

													if (newpass && j == 0) process = true;
													else if (j > 0) process = true;

													if (process) {
														if (on_itr->second->v00()->getUID() == midPoint->getUID()) {
															_nedge = mesh()->addEdge(centralPoint, on_itr->second->v01());
														}
														else if (on_itr->second->v01()->getUID() == midPoint->getUID()) {
															_nedge = mesh()->addEdge(on_itr->second->v00(), centralPoint);
														}

														if (_nedge != nullptr) {
															
															util::removeEdge(on_itr->second->getUID(), output);
															mesh()->removeEdge(on_itr->second);
									
															newOldBevelEdge[mid[j].reference()->getUID()] = _nedge;
														
															output.push_back(_nedge);
														}

														skip = true;
													}
												}

												if (mid[j].a()->getUID() == midPoint->getUID() && !skip) {
													_nedge = mesh()->addEdge(centralPoint, mid[j].b());						
												}
												else if (mid[j].b()->getUID() == midPoint->getUID() && !skip) {
													_nedge = mesh()->addEdge(mid[j].a(), centralPoint);
												}

												//std::pair<size_t, Edge*> p(mid[j].reference()->getUID(), _nedge);
		
												/*if (j == 0 && i > 0) {
													auto r_itr = edgeMap.find(edgeLoop[i - 1].reference()->getUID()));
												}*/
												newOldBevelEdge[mid[j].reference()->getUID()] = _nedge;
												output.push_back(_nedge);
												
												if (j > 0) {
													auto sharedFace = util::sharedFace(mid[j - 1].reference(), mid[j].reference());

													if (sharedFace != nullptr) {
														//garbageFaces.push_back(sharedFace);
														EdgeList el;
														std::vector<Vert*> vts;
														for (auto srt : util::sortFaceEdges(sharedFace)) {

															auto ne_itr = newOldBevelEdge.find(srt.reference()->getIndex());

															if (ne_itr != newOldBevelEdge.end()) {

																el.push_back(ne_itr->second);

																if (srt.a()->getUID() == midPoint->getUID()) {
																	if (ne_itr->second->v00()->getUID() == centralPoint->getUID()) {
																		vts.push_back(ne_itr->second->v00());
																	}
																	else if (ne_itr->second->v01()->getUID() == centralPoint->getUID()) {
																		vts.push_back(ne_itr->second->v01());
																	}
																	else {
																		printf("\nFailed to find vertex...00\n");
																	}
																}
																else if (srt.b()->getUID() == midPoint->getUID()) {
																	if (ne_itr->second->v00()->getUID() == centralPoint->getUID()) {
																		vts.push_back(ne_itr->second->v01());
																	}
																	else if (ne_itr->second->v01()->getUID() == centralPoint->getUID()) {
																		vts.push_back(ne_itr->second->v00());
																	}
																	else {
																		printf("\nFailed to find vertex...01\n");
																	}
																}
																else if(srt.a()->getUID() == other->getUID()) {
																	auto c_itr = oldNewMidPoint.find(other->getUID());

																	if (ne_itr->second->v00()->getUID() == c_itr->second->getUID()) {
																		vts.push_back(ne_itr->second->v00());
																	}
																	else if (ne_itr->second->v01()->getUID() == c_itr->second->getUID()) {
																		vts.push_back(ne_itr->second->v01());
																	}
																	else {
																		printf("\nFailed to find vertex...02\n");
																	}
																}
																else if (srt.b()->getUID() == other->getUID()) {
																	auto c_itr = oldNewMidPoint.find(other->getUID());

																	if (ne_itr->second->v00()->getUID() == c_itr->second->getUID()) {
																		vts.push_back(ne_itr->second->v01());
																	}
																	else if (ne_itr->second->v01()->getUID() == c_itr->second->getUID()) {
																		vts.push_back(ne_itr->second->v00());
																	}
																	else {
																		printf("\nFailed to find vertex...03\n");
																	}
																}
																														
															}
															else {
																el.push_back(srt.reference());
																vts.push_back(srt.a());
															}

															//vts.push_back(el.back()->v00());
														}

														BevelTempFace btemp;
														btemp.original_uid = sharedFace->getUID();
														btemp.ls = el;
														btemp.vts = vts;
														newFaceReference[btemp.original_uid] = btemp;
													}
												}
												
												newpass = false;
											}

											mid.clear();
											mid.push_back(edgeLoop[i]);
										}

									}

								}
							}

							std::pair<size_t, Vert*> p(midPoint->getUID(), midPoint);
							visited_midpoints.insert(p);
						}
					}
				}
				cnt++;
			}

			for (auto temp : newFaceReference) {
				//mesh()->addFace(temp.second.ls, temp.second.vts);
				printf("Face edge # %i\n", temp.second.ls.size());
				for (auto ee : temp.second.ls) {
					printf("id(%i): %i, %i\n", ee->getUID(), ee->v00()->getUID(), ee->v01()->getUID());
				}		
				for (auto v : temp.second.vts) printf("vt: %i\n", v->getUID());
				printf("\n\n");
			}


			// Create Faces
			//visited_midpoints.clear();
			//for (auto e : es) {
			//	std::vector<std::pair<Vert*, Vert*>> midPoints;
			//	//Vert *other = nullptr;
			//	bool hasMidpoint = false;

			//	if (!util::isEndPoint(e->v00(), es)) {
			//		std::pair<Vert*, Vert*> p(e->v00(), e->v01());
			//		midPoints.push_back(p);
			//		hasMidpoint = true;
			//	}
			//	else if (!util::isEndPoint(e->v01(), es)) {
			//		std::pair<Vert*, Vert*> p(e->v01(), e->v00());
			//		midPoints.push_back(p);
			//		hasMidpoint = true;
			//	}

			//	if (hasMidpoint) {
			//		for (auto mid_pair : midPoints) {
			//			auto midPoint = mid_pair.first;
			//			auto other = mid_pair.second;

			//			auto itr = visited_midpoints.find(midPoint->getUID());

			//			if (itr == visited_midpoints.end()) {
			//				printf("Midpoint...\n");
			//				auto edgeLoop = util::pointAdjacentEdgesCCW(e, midPoint, other);

			//				if (edgeLoop.size() > 1) {

			//					std::vector<util::ProxyEdge> mid;
			//					for (int i = 0; i < edgeLoop.size(); i++) {

			//						if (i > 0 && edgeLoop[i].reference()->getUID() == e->getUID()) break;

			//						mid.push_back(edgeLoop[i]);
			//						auto itr = edgeMap.find(edgeLoop[i].reference()->getUID());

			//						if (itr != edgeMap.end()) {

			//							if (mid.size() >= 2) {

			//								for (int j = 1; j < mid.size(); j++) {
			//									auto sharedFace = util::sharedFace(mid[j - 1].reference(), mid[j].reference());

			//									if (sharedFace != nullptr) {
			//										garbageFaces.push_back(sharedFace);
			//										EdgeList el;
			//										std::vector<Vert*> vts;
			//										for (auto srt : util::sortFaceEdges(sharedFace)) {

			//											auto ne_itr = newOldBevelEdge.find(srt.reference()->getIndex());

			//											if (ne_itr != newOldBevelEdge.end()) {
			//												el.push_back(ne_itr->second);

			//												/*if (srt.a()->getUID() == midPoint->getUID()) {
			//													if (ne_itr->second->v00()->getUID() == centralPoint->getUID()) {
			//														vts.push_back(ne_itr->second->v00());
			//													}
			//													else if (ne_itr->second->v01()->getUID() == centralPoint->getUID()) {
			//														vts.push_back(ne_itr->second->v01());
			//													}
			//												}

			//												if (srt.b()->getUID() == midPoint->getUID()) {
			//													if (ne_itr->second->v00()->getUID() == centralPoint->getUID()) {
			//														vts.push_back(ne_itr->second->v01());
			//													}
			//													else if (ne_itr->second->v01()->getUID() == centralPoint->getUID()) {
			//														vts.push_back(ne_itr->second->v00());
			//													}
			//												}*/
			//											}
			//											else {
			//												el.push_back(srt.reference());
			//												vts.push_back(srt.a());
			//											}

			//											//vts.push_back(el.back()->v00());
			//										}

			//										printf("Face edge # %i\n", el.size());
			//										for (auto ee : el) {
			//											//mesh()->addFace(el, vts);
			//											printf("%i, %i\n", ee->v00()->getUID(), ee->v01()->getUID());
			//										}
			//										for (auto v : vts) printf("vt: %i\n", v->getUID());
			//										printf("\n\n");
			//									}
			//								}
			//							}
			//						}
			//					}
			//				}

			//				std::pair<size_t, Vert*> p(midPoint->getUID(), midPoint);
			//				visited_midpoints.insert(p);
			//			}
			//		}
			//	}
			//}
			// Solve end Points
			//for (auto e : es) {
			//	auto ifs = mq.edgeIncidentFaces(e);
			//	std::vector<Edge*> splitEdges;
			//	for (auto vt : mq.edgeAdjacentVertices(e)) {

			//		if (util::isEndPoint(vt, es)) {

			//			for (auto e_fs : mq.vertexIncidentFaces(vt)) {

			//				if (e_fs->getUID() != ifs.front()->getUID() &&
			//					e_fs->getUID() != ifs.back()->getUID()) {

			//					glm::vec3 splitDir00(0);
			//					glm::vec3 splitDir01(0);
			//					ds::Vert *split00 = nullptr;
			//					ds::Vert *split01 = nullptr;
			//					ds::Vert *other00 = nullptr;
			//					ds::Vert *other01 = nullptr;
			//					ds::Edge *oldEdge00 = nullptr;
			//					ds::Edge *oldEdge01 = nullptr;
			//					bool edge00_exists = false;
			//					bool edge01_exists = false;

			//					for (auto e_fs_e : util::sortFaceEdges(e_fs)) {
			//						auto ed_check = newOldBevelEdge.find(e_fs_e.reference()->getUID());

			//						if (ed_check != newOldBevelEdge.end()) {
			//							if (vt->getUID() == e_fs_e.a()->getUID()) {
			//								split00 = ed_check->second->v01();
			//								other00 = ed_check->second->v00();

			//								edge00_exists = true;
			//							}
			//							else if (vt->getUID() == e_fs_e.b()->getUID()) {
			//								split01 = ed_check->second->v00();
			//								other01 = ed_check->second->v01();

			//								edge01_exists = true;
			//							}
			//						}
			//						else {
			//							if (vt->getUID() == e_fs_e.a()->getUID()) {
			//								split00 = mesh()->addVertex(vt->position());
			//								other00 = e_fs_e.b();

			//								splitDir00 = glm::normalize(e_fs_e.a()->position() - e_fs_e.b()->position());

			//								oldEdge00 = e_fs_e.reference();
			//							}
			//							else if (vt->getUID() == e_fs_e.b()->getUID()) {
			//								split01 = mesh()->addVertex(vt->position());
			//								other01 = e_fs_e.a();

			//								splitDir01 = glm::normalize(e_fs_e.a()->position() - e_fs_e.b()->position());

			//								oldEdge01 = e_fs_e.reference();
			//							}
			//						}
			//					}

			//					if (split00 != nullptr &&
			//						split01 != nullptr &&
			//						other00 != nullptr &&
			//						other01 != nullptr) {

			//						Edge *splitEdge = mesh()->addEdge(split00, split01);
			//						splitEdges.push_back(splitEdge);

			//						output.push_back(splitEdge);
			//						if (!edge00_exists) {
			//							Edge *edge00 = mesh()->addEdge(split00, other00);

			//							std::pair<size_t, Edge*> p(oldEdge00->getUID(), edge00);
			//							newOldBevelEdge.insert(p);

			//							split00->position() = split00->position() + (-(offset*1.f) * splitDir00);
			//							//printf("New Edge Created 00...\n");
			//						}

			//						if (!edge01_exists) {
			//							Edge *edge01 = mesh()->addEdge(other01, split01);

			//							std::pair<size_t, Edge*> p(oldEdge01->getUID(), edge01);
			//							newOldBevelEdge.insert(p);

			//							split01->position() = split01->position() + ((offset*1.f) * splitDir01);
			//							//printf("New Edge Created 01...\n");
			//						}
			//						/*Vert *a = mesh()->addVertex(vt->position());
			//						Vert *b = mesh()->addVertex(vt->position());*/
			//					}
			//					else {
			//						printf("Bevel: Failed end point split\n");
			//					}
			//				}
			//			}
			//		}
			//	}
			//}
			
			for (auto f : garbageFaces) {

				for (auto el : f->adjacentEdges()) {
					bool exists = false;

					for (auto el00 : garbageEdges) if (el00->getUID() == el->getUID()) exists = true;

					if(!exists) garbageEdges.push_back(el);
				}
			}

			/*for (auto f : garbageFaces) mesh()->removeFace(f);

			for (auto e : garbageEdges) {
				if (e->incidentFaces().empty()) mesh()->removeEdge(e);
			}

			for (auto v : garbageVerts) mesh()->removeVertex(v);*/
			
		}
		
		mesh()->reindexVertices();
		mesh()->reindexFaces();
		mesh()->reindexEdges();

		return output;
	}

	// Bevel Edges
	std::vector<Edge*> MeshOperator::bevelEdges02(std::vector<size_t> edges, float offset) {
		MeshQuery mq;
		std::vector<Edge*> output;
		std::unordered_map<size_t, Edge*> edgeMap;
		std::unordered_map<size_t, BeveledEdge *> oldToBeveledEdge;
		std::vector<BeveledEdge*> beveledEdges;
	
		if (!edges.empty()) {
			
			auto es = edgesFromIndices(edges);

			for (auto e : es) {
				auto ifs = mq.edgeIncidentFaces(e);
				if (ifs.size() != 2) return output;

				std::pair<size_t, Edge*> pair(e->getUID(), e);
				edgeMap.insert(pair);
			}
		
			for (auto e : es) {
				auto ifs = mq.edgeIncidentFaces(e);

				BeveledEdge *be = new BeveledEdge;
				be->originalEdge = e;
				be->originalAdjacentFace_a = ifs.front();
				be->originalAdjacentFace_b = ifs.back();
				be->v00a = mesh()->addVertex(e->v00()->position());
				be->v01a = mesh()->addVertex(e->v01()->position());
				be->v00b = mesh()->addVertex(e->v00()->position());
				be->v01b = mesh()->addVertex(e->v01()->position());
				be->hardEdgeA = mesh()->addEdge(be->v00a, be->v01a);
				be->hardEdgeB = mesh()->addEdge(be->v00b, be->v01b);

				output.push_back(be->hardEdgeA);
				output.push_back(be->hardEdgeB);

				glm::vec3 u_a(be->originalAdjacentFace_a->vertices()[1]->position() - be->originalAdjacentFace_a->vertices()[0]->position());
				glm::vec3 v_a(be->originalAdjacentFace_a->vertices()[2]->position() - be->originalAdjacentFace_a->vertices()[0]->position());

				glm::vec3 u_b(be->originalAdjacentFace_b->vertices()[1]->position() - be->originalAdjacentFace_b->vertices()[0]->position());
				glm::vec3 v_b(be->originalAdjacentFace_b->vertices()[2]->position() - be->originalAdjacentFace_b->vertices()[0]->position());

				glm::vec3 normi_a = glm::normalize(glm::cross(u_a, v_a));
				glm::vec3 normi_b = glm::normalize(glm::cross(u_b, v_b));

				auto prx_a = util::adjacentFaceProxy(be->originalAdjacentFace_a, e);
				auto prx_b = util::adjacentFaceProxy(be->originalAdjacentFace_b, e);

				glm::vec3 dispDir_a = glm::normalize(glm::cross(normi_a, (prx_a.b()->position() - prx_a.a()->position())));
				glm::vec3 dispDir_b = glm::normalize(glm::cross(normi_b, (prx_b.b()->position() - prx_b.a()->position())));

				be->hardEdgeA->v00()->position() += (offset * dispDir_a);
				be->hardEdgeA->v01()->position() += (offset * dispDir_a);

				auto rc = util::distance(be->hardEdgeA, be->originalEdge);

				printf("Offset: %f, Distance from edge: %f\n\n", offset, rc.first);

				be->hardEdgeB->v00()->position() += (offset * dispDir_b);
				be->hardEdgeB->v01()->position() += (offset * dispDir_b);

				oldToBeveledEdge[e->getUID()] = be;
				beveledEdges.push_back(be);
			}

			for (auto be00 : beveledEdges) {
				for (auto be01 : beveledEdges) {
					if (be00->originalEdge->getUID() != be01->originalEdge->getUID()) {

						auto rc00 = util::distance(be00->hardEdgeA, be01->hardEdgeA);
						auto rc01 = util::distance(be00->hardEdgeA, be01->hardEdgeB);
						auto rc02 = util::distance(be00->hardEdgeB, be01->hardEdgeA);
						auto rc03 = util::distance(be00->hardEdgeB, be01->hardEdgeB);

						printf("Distance 00: %f\n", rc00.first);
						printf("Distance 01: %f\n", rc01.first);
						printf("Distance 02: %f\n", rc02.first);
						printf("Distance 03: %f\n", rc03.first);
					}
				}
			}
		}

		mesh()->reindexVertices();
		mesh()->reindexFaces();
		mesh()->reindexEdges();

		return output;
	}
	
}