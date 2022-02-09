import argparse
from xml.etree.ElementTree import Element
import xml.etree.ElementTree as ET
import logging



def convert(tree: ET.ElementTree) -> list[str]:
    view_nodes = tree.findall("view")
    views = convert_views(view_nodes)
    modules = []
    calls = []
    params = []
    for view in view_nodes:
        converteds = convert_modules(view.findall("module"))
        modules += converteds[0]
        params += converteds[1]
        calls += convert_calls(view.findall("call"))
    logging.info(f"Module Graph contains {len(views)} views, {len(modules)} modules, {len(calls)} calls, and {len(params)} params.")
    lines = []
    lines += views
    lines += modules
    lines += calls
    lines += params
    return lines


def convert_views(views: list[Element]) -> list[str]:
    # mmCreateView("GraphEntry_1","View3DGL","::RaycastVolumeExample::View3DGL1")
    result = []
    for view in views:
        viewmod = view.findall(f"module[@name='{view.get('viewmod')}']")[0]
        result.append(f"mmCreateView(\"{view.get('name')}\", \"{viewmod.get('class')}\", \"::{viewmod.get('name')}\")")
    return result


def convert_modules(module_nodes: list[Element]) -> list[list[str]]:
    # mmCreateModule("ScreenShooter","::RaycastVolumeExample::ScreenShooter1")
    modules = []
    params = []
    for module in module_nodes:
        module_str = f"mmCreateModule(\"{module.get('class')}\", \"::{module.get('name')}\")"
        modules.append(module_str)
        logging.debug(module_str)
        for param in module.findall("param"):
            # mmSetParamValue("::RaycastVolumeExample::ScreenShooter1::view",[=[::RaycastVolumeExample::View3DGL1]=])
            param_str = f"mmSetParamValue(\"::{module.get('name')}::{param.get('name')}\",[=[{param.get('value')}]=])"
            params.append(param_str)
            logging.debug(param_str)
    return [modules, params]


def convert_calls(calls: list[Element]) -> list[str]:
    # mmCreateCall("VolumetricDataCall","::RaycastVolumeExample::RaycastVolumeRenderer1::getData","::RaycastVolumeExample::VolumetricDataSource1::GetData")
    result = []
    for call in calls:
        result.append(f"mmCreateCall(\"{call.get('class')}\", \"::{call.get('from')}\", \"::{call.get('to')}\")")
    return result


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Converts a given xml-based mmprj MegaMol project file to the new lua-based definition.")
    parser.add_argument("input", help=".mmprj file to convert")
    parser.add_argument("-v", "--verbosity", action="count", default=0)
    args = parser.parse_args()

    logging.basicConfig()

    if args.verbosity == 1:
        logging.getLogger().setLevel(logging.INFO)
    elif args.verbosity > 1:
        logging.getLogger().setLevel(logging.DEBUG)

    tree = ET.parse(args.input)
    lua_contents = convert(tree)

    with open(args.input.replace(".mmprj", ".lua"), "w") as the_file:
        [print(l, file=the_file) for l in lua_contents]