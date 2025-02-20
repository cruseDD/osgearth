/* -*-c++-*- */
/* osgEarth - Geospatial SDK for OpenSceneGraph
* Copyright 2020 Pelican Mapping
* http://osgearth.org
*
* osgEarth is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#include <osgViewer/Viewer>
#include <osgEarth/EarthManipulator>
#include <osgEarth/ExampleResources>
#include <osgEarth/MapNode>
#include <osgEarth/PhongLightingEffect>
#include <osgGA/TrackballManipulator>
#include <iostream>
#include <osgEarth/PlaceNode>
#include <osgEarth/ObjectIDPicker>

#include <osgEarth/Metrics>

#define LC "[viewer] "

using namespace osgEarth;
using namespace osgEarth::Util;

int
usage(const char* name)
{
    std::cout
        << "\nUsage: " << name << " file.earth" << std::endl
        << Util::MapNodeHelper().usage() << std::endl;

    return 0;
}


int
main(int argc, char** argv)
{
    osg::ArgumentParser arguments(&argc,argv);
    if ( arguments.read("--help") )
        return usage(argv[0]);

    osgEarth::initialize(arguments);
    osgViewer::Viewer viewer(arguments);
    viewer.setCameraManipulator(new EarthManipulator(arguments));
    viewer.getCamera()->setSmallFeatureCullingPixelSize(-1.0f);

    auto node = MapNodeHelper().load(arguments, &viewer);
    if (node.valid())
    {
        auto mapnode = MapNode::get(node);
        if (mapnode)
        {
            auto anno = new PlaceNode();
            anno->setText("Hello, World!");
            anno->setPosition(GeoPoint(mapnode->getMapSRS(), -122.5, 45.5));
            Registry::objectIndex()->tagNode(anno, anno);
            mapnode->addChild(anno);

            auto picker = new ObjectIDPicker();
            picker->setView(&viewer);
            picker->setGraph(mapnode);
            mapnode->addChild(picker);

            picker->onClick([&](const ObjectID& id)
                {
                    if (id != OSGEARTH_OBJECTID_EMPTY)
                    {
                        auto place = Registry::objectIndex()->get<AnnotationNode>(id);
                        if (place)
                        {
                            std::cout << "Clicked on \"" << place->getText() << "\"" << std::endl;
                        }
                    }
                });


            viewer.setSceneData(node);
            return viewer.run();
        }
    }

    return usage(argv[0]);
}