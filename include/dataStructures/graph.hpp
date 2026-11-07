#ifndef JFLX_GRAPH_HPP
#define JFLX_GRAPH_HPP

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

namespace Graph {
    class Vertex {
        public:
            /*
            Ein neues Objekt vom Typ Vertex mit der ID pID wird erstellt. Seine Markierung hat den
            Wert false.
            */
            Vertex(std::string pID = "", bool pIsMarked = false) : ID(pID), mark(pIsMarked) {

            };

            /*
            Der Auftrag setzt die Markierung des Knotens auf den Wert isMarked
            */
            void setMark(bool pIsMarked = false) {
                mark = pIsMarked;
            }

            /*
            Die Anfrage liefert die ID des Knotens als String.
            */
            std::string getID() const  {
                return ID;
            }

            /*
            Die Anfrage liefert true, wenn die Markierung des Knotens den Wert true hat, ansonsten
            false.
            */
            bool isMarked() const {
                return mark;
            }

            bool operator==(const Vertex& other) const {
                return (
                    ID    == other.getID() &&
                    mark  == other.isMarked()
                );
            }

        private:
            std::string ID;
            bool mark;
    };

    class Edge {
        public:
            /*
            Ein neues Objekt vom Typ Edge wird erstellt. Die von diesem Objekt repräsentierte Kante
            verbindet die Knoten pVertex und pAnotherVertex mit der Gewichtung pWeight. Ihre
            Markierung hat den Wert false.
            */
            Edge(Vertex pVertexA, Vertex pVertexB, double pWeight = 0.0, bool pIsMarked = false) : mark(pIsMarked), weight(pWeight), vertexA(pVertexA), vertexB(pVertexB) {

            }

            /*
            Der Auftrag setzt das Gewicht der Kante auf den Wert pWeight.
            */
            void setWeight(bool pWeight) {
                weight = pWeight;
            }

            /*
            Der Auftrag setzt die Markierung der Kante auf den Wert isMarked.
            */
            void setMark(bool pIsMarked = false) {
                mark = pIsMarked;
            }

            /*
            Die Anfrage liefert das Gewicht der Kante als double.
            */
            double getWeight() const  {
                return weight;
            }

            /*
            Die Anfrage liefert true, wenn die Markierung der Kante den Wert true hat, ansonsten
            false.
            */
            bool isMarked() const  {
                return mark;
            }

            /*
            Die Anfrage gibt die beiden Knoten, die durch die Kante verbunden werden, als neues Feld
            vom Typ Vertex zurück. Das Feld hat genau zwei Einträge mit den Indexwerten 0 und 1.
            */
            std::vector<Vertex> getVertices() const {
                return {vertexA, vertexB};
            }

            bool operator==(const Edge& other) const {
                return (
                    vertexA == other.getVertices()[0] &&
                    vertexA == other.getVertices()[1] &&
                    weight  == other.getWeight() &&
                    mark    == other.isMarked()
                );
            }
        private:
            bool mark;
            double weight;
            Vertex vertexA;
            Vertex vertexB;
    };

    class Graph {
        public:
            /*
            Ein Objekt vom Typ Graph wird erstellt. Der von diesem Objekt repräsentierte Graph ist
            leer.
            */
            Graph() {

            };

            /*
            Der Auftrag fügt den Knoten pVertex vom Typ Vertex in den Graphen ein, sofern es noch
            keinen Knoten mit demselben ID-Eintrag wie pVertex im Graphen gibt und pVertex eine
            ID ungleich null hat. Ansonsten passiert nichts.
            Gibt zurück ob der Knoten eingefügt werden konnte.
            */
            bool addVertex(Vertex pVertex) {
                if (!contains(pVertex)) {
                    vertices.push_back(pVertex);
                    return true;
                } return false;
            }

            /*
            Der Auftrag fügt die Kante pEdge in den Graphen ein, sofern beide durch die Kante 
            verbundenen Knoten im Graphen enthalten sind, nicht identisch sind und noch keine 
            Kante zwischen den beiden Knoten existiert. Ansonsten passiert nichts.
            Gibt zurück ob die Kante eingefügt werden konnte.
            */
            bool addEdge(Edge pEdge) {
                if (!contains(pEdge)) {
                    edges.push_back(pEdge);
                    return true;
                } return false;
            }

            /*
            Der Auftrag entfernt den Knoten pVertex aus dem Graphen und löscht alle Kanten, die mit
            ihm inzident sind. Ist der Knoten pVertex nicht im Graphen enthalten, passiert nichts.
            Gibt zurück ob der Knoten entfernt werden konnte.
            */
            bool removeVertex(Vertex pVertex) {
                for(auto it = edges.cbegin(); it != edges.cend();) {
                    std::vector<Vertex> pVertices = it->getVertices();
                    if (pVertices[0] == pVertex || pVertices[1] == pVertex) {
                        it = edges.erase(it);
                    } else {
                         it ++;
                    }
                }

                for(auto it = vertices.cbegin(); it != vertices.cend(); it ++) {
                    if (*(it) == pVertex) {
                        vertices.erase(it);
                        return true;
                    }
                }
                return false;
            }

            /*
            Der Auftrag entfernt die Kante pEdge aus dem Graphen. 
            Ist die Kante pEdge nicht im Graphen enthalten, passiert nichts.
            Gibt zurück ob die Kante entfernt werden konnte.
            */
            bool removeEdge(Edge pEdge) {
                for(auto it = edges.cbegin(); it != edges.cend(); it ++) {
                    if (*(it) == pEdge) {
                        edges.erase(it);
                        return true;
                    }
                }
                return false;
            }

            /*
            Die Anfrage liefert das Knotenobjekt mit pID als ID. Ist ein solches Knotenobjekt nicht im
            Graphen enthalten, wird null zurückgeliefert
            */
            Vertex getVertex(std::string pID) {
                for(Vertex vertex : vertices) {
                    if (vertex.getID() == pID) {
                        return vertex;
                    }
                }
            }

            /*
            Die Anfrage liefert alle Nachbarn des Knotens pVertex als neue Liste vom Typ
            List<Vertex>. Hat der Knoten pVertex keine Nachbarn in diesem Graphen oder ist gar
            nicht in diesem Graphen enthalten, so wird eine leere Liste zurückgeliefert.
            */
            std::vector<Vertex> getNeighbours(Vertex pVertex) {
                std::vector<Vertex> neighbours;

                for (Edge edge : edges) {
                    std::vector<Vertex> pVertices = edge.getVertices();
                    if (pVertices[0] == pVertex) {
                        if(std::find(neighbours.begin(), neighbours.end(), pVertices[1]) == neighbours.end()) {
                            neighbours.push_back(pVertices[1]);
                        }
                    } else if (pVertices[1] == pVertex) {
                        if(std::find(neighbours.begin(), neighbours.end(), pVertices[0]) == neighbours.end()) {
                            neighbours.push_back(pVertices[0]);
                        }
                    }
                }

                return neighbours;
            }

            /*
            Die Anfrage liefert eine neue Liste aller Kantenobjekte vom Typ List<Edge>. Enthält der
            Graph keine Kantenobjekte, so wird eine leere Liste zurückgeliefert.
            */
            std::vector<Edge> getEdges() {
                return edges;
            }

            /*
            Die Anfrage liefert alle Nachbarn des Knotens pVertex als neue Liste vom Typ
            List<Vertex>. Hat der Knoten pVertex keine Nachbarn in diesem Graphen oder ist gar
            nicht in diesem Graphen enthalten, so wird eine leere Liste zurückgeliefert.
            */
            std::vector<Edge> getEdges(Vertex pVertex) {
                std::vector<Edge> connectedEdges;

                for (Edge edge : edges) {
                    std::vector<Vertex> pVertices = edge.getVertices();
                    if (pVertices[0] == pVertex || pVertices[1] == pVertex) {
                        if(std::find(connectedEdges.begin(), connectedEdges.end(), edge) == connectedEdges.end()) {
                            connectedEdges.push_back(edge);
                        }
                    }
                }

                return connectedEdges;
            }

            /*
            Die Anfrage liefert die Kante, welche die Knoten pVertex und pAnotherVertex verbindet, als Objekt vom Typ Edge. Ist der Knoten pVertex oder der Knoten pAnotherVertex
            nicht im Graphen enthalten oder gibt es keine Kante, die beide Knoten verbindet, so wird
            null zurückgeliefert.
            */
            Edge getEdge(Vertex pVertexA, Vertex pVertexB) {
                for (Edge edge : edges) {
                    std::vector<Vertex> pVertices = edge.getVertices();
                    if ((pVertices[0] == pVertexA && pVertices[1] == pVertexB) ||
                        (pVertices[0] == pVertexB && pVertices[1] == pVertexA)) {
                        return edge;
                    }
                }
            }



            /*
            Der Auftrag setzt die Markierungen aller Knoten des Graphen auf den Wert isMarked. 
            */
            void setAllVertexMarks(bool isMarked = false) {
                for (Vertex vertex : vertices) {
                    vertex.setMark(isMarked);
                }
            }

            /*
            Die Anfrage liefert true, wenn die Markierungen aller Knoten des Graphen den Wert von mark
            haben, ansonsten false.
            */
            bool allVerticesMarked(bool mark = true) {
                for (Vertex vertex : vertices) {
                    if (vertex.isMarked() != mark) {
                        return false;
                    }
                } return true;
            }

            /*
            Der Auftrag setzt die Markierungen aller Kanten des Graphen auf den Wert isMarked
            */
            void setAllEdgeMarks(bool isMarked) {
                for (Edge edge : edges) {
                    edge.setMark(isMarked);
                }
            }

            
            /*
            Die Anfrage liefert true, wenn die Markierungen aller Kanten des Graphen den Wert von mark
            haben, ansonsten false.
            */
            bool allEdgesMarked(bool mark = true) {
                for (Edge edge : edges) {
                    if (edge.isMarked() != mark) {
                        return false;
                    }
                } return true;
            }

            /*
            Die Anfrage liefert true, wenn der Graph keine Knoten enthält, ansonsten false.
            */
            bool isEmpty() {
                return vertices.empty();
            }



        private:
            std::vector<Edge>   edges;
            std::vector<Vertex> vertices;

            bool contains(Vertex pVertex) {
                return std::find(vertices.begin(), vertices.end(), pVertex) != vertices.end();
            }

            bool contains(Edge pEdge) {
                return std::find(edges.begin(), edges.end(), pEdge) != edges.end();
            }
    };
}

#endif // JFLX_GRAPH_HPP