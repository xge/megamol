import logging
from xml.etree import ElementTree as ET
from xml.etree.ElementTree import Element


class Converter:
    def __init__(self, filename: str):
        self.root = ET.parse(filename)
        self.viewmodname: str = None
        self.view: str = None
        self.modules: list[str] = []
        self.params: list[str] = []
        self.calls: list[str] = []
        self.convert_view()
        self.convert_calls()
        self.convert_modules()

    @staticmethod
    def sanitize(classname: str) -> str:
        mappings = {
            "View2D": "View2DGL",
            "View3D": "View3DGL",
            "SplitView": "SplitViewGL",
            "CallGetTransferFunction": "CallGetTransferFunctionGL",
            "CallRender2D": "CallRender2DGL",
            "CallRender3D": "CallRender3DGL",
            "CallRenderView": "CallRenderViewGL",
            "SphereOutlineRenderer": "SphereRenderer",
            "TransferFunction": "TransferFunctionGL",
            "FlagStorage": "UniFlagStorageGL"
        }
        if classname in mappings:
            return mappings[classname]
        else:
            return classname

    @staticmethod
    def additional_module_params(classname: str, modulename: str) -> list[str]:
        mappings = {
            "SphereOutlineRenderer": "mmSetParamValue(\"::{modulename}::renderMode\",[=[Outline]=])"
        }
        if classname in mappings:
            return [mappings[classname].format(modulename=modulename)]
        else:
            return []

    def to_lua(self) -> list[str]:
        """
        TODO: handle missing lights
        TODO: handle Mux*Renderer3D
        TODO: handle BBox
        TODO: fix transfer/tranfser mess
        :param tree:
        :return:
        """
        logging.info(
            f"Module Graph contains {len(self.view)} views, {len(self.modules)} modules, {len(self.calls)} calls, and {len(self.params)} params.")
        lines = []
        lines += ["-- VIEW"]
        lines += self.view
        lines += ["-- MODULES"]
        lines += self.modules
        lines += ["-- CALLS"]
        lines += self.calls
        lines += ["-- PARAMETERS"]
        lines += self.params
        return lines

    def convert_view(self):
        # mmCreateView("GraphEntry_1","View3DGL","::RaycastVolumeExample::View3DGL1")
        result = []
        for view in self.root.findall("view"):
            viewmod = view.findall(f"module[@name='{view.get('viewmod')}']")[0]
            if viewmod.get("class") == "GUIView":
                calls = view.findall(f"call[@class='CallRenderView']")
                calls = list(filter(lambda c: c.get("from").startswith("GUIView"), calls))
                assert (len(calls) == 1)
                viewmod = calls[0]

                name = view.get("name")
                modulename = viewmod.get("to").split("::")[0]
                classname = view.find(f"module[@name='{modulename}']").get("class")
                self.viewmodname = modulename

                result.append(f"mmCreateView(\"{name}\", \"{Converter.sanitize(classname)}\", \"::{modulename}\")")
            else:
                self.viewmodname = viewmod.get("name")
                result.append(
                    f"mmCreateView(\"{view.get('name')}\", \"{Converter.sanitize(viewmod.get('class'))}\", \"::{viewmod.get('name')}\")")
        self.view = result

    def convert_modules(self):
        # mmCreateModule("ScreenShooter","::RaycastVolumeExample::ScreenShooter1")
        module_nodes = self.root.find("view").findall("module")
        modules = []
        params = []
        for module in module_nodes:
            if module.get("class") == "GUIView":
                continue

            # prevent viewmod add
            if module.get("name") == self.viewmodname:
                continue

            classname = Converter.sanitize(module.get("class"))
            module_str = f"mmCreateModule(\"{classname}\", \"::{module.get('name')}\")"
            modules.append(module_str)
            logging.debug(module_str)

            params += Converter.additional_module_params(module.get("class"), module.get("name"))

            for param in module.findall("param"):
                # mmSetParamValue("::RaycastVolumeExample::ScreenShooter1::view",[=[::RaycastVolumeExample::View3DGL1]=])
                blacklist = ['showBBox', 'bboxCol', 'replacmentKeyAssign']
                mappings = {
                    'vxcolumnname': "::{modulename}::direction::{paramname}",
                    'vycolumnname': "::{modulename}::direction::{paramname}",
                    'vzcolumnname': "::{modulename}::direction::{paramname}",
                    'replacementRendering': "::{modulename}::replacement"
                }

                if param.get('name') not in blacklist:
                    paramname = f"::{module.get('name')}::{param.get('name')}"
                    if param.get('name') in mappings:
                        paramname = mappings[param.get('name')].format(modulename=module.get('name'),
                                                                       paramname=param.get('name'))

                    param_str = f"mmSetParamValue(\"{paramname}\",[=[{param.get('value')}]=])"
                    params.append(param_str)
                    logging.debug(param_str)

        self.modules = modules
        self.params = params

    def convert_calls(self):
        # mmCreateCall("VolumetricDataCall","::RaycastVolumeExample::RaycastVolumeRenderer1::getData","::RaycastVolumeExample::VolumetricDataSource1::GetData")
        def default_converter(call: Element) -> str:
            return f"mmCreateCall(\"{Converter.sanitize(call.get('class'))}\", \"::{call.get('from')}\", \"::{call.get('to')}\")"

        def CallKeyframeKeeper_converter(call: Element) -> str:
            """Slots have to be named `keyframeData`."""
            classname = Converter.sanitize(call.get("class"))

            fromname = call.get("from").split("::")[:-1]
            fromname.append("keyframeData")
            fromname = "::".join(fromname)

            toname = call.get("to").split("::")[:-1]
            toname.append("keyframeData")
            toname = "::".join(toname)

            return f"mmCreateCall(\"{classname}\", \"::{fromname}\", \"::{toname}\")"

        def CallRender3D_converter(call: Element) -> str:
            """Some slots must be renamed (e.g., TrackingShotRenderer)."""

            classname = Converter.sanitize(call.get("class"))
            fromname = call.get("from")
            toname = call.get("to")

            if "TrackingShotRenderer" in fromname:
                fromname = fromname.split("::")[:-1]
                fromname.append("chainRendering")
                fromname = "::".join(fromname)
            elif "PreviewReplacement" in fromname:
                fromname = fromname.split("::")[:-1]
                fromname.append("chainRendering")
                fromname = "::".join(fromname)
            elif "PositioningReplacement" in fromname:
                fromname = fromname.split("::")[:-1]
                fromname.append("chainRendering")
                fromname = "::".join(fromname)

            return f"mmCreateCall(\"{classname}\", \"::{fromname}\", \"::{toname}\")"

        def FlagCall_converter(call: Element) -> str:
            """For the moment we're transforming to READ calls only."""
            classname = "FlagCallRead_GL"

            fromname = call.get("from")
            slotName = "readFlags"
            if "ArrowRenderer" in fromname:
                slotName = "getflags"
            fromname = fromname.split("::")[:-1]
            fromname.append(slotName)
            fromname = "::".join(fromname)

            toname = call.get("to")
            toname = toname.split("::")[:-1]
            toname.append("readFlags")
            toname = "::".join(toname)

            return f"mmCreateCall(\"{classname}\", \"::{fromname}\", \"::{toname}\")"

        mappings = {
            "CallKeyframeKeeper": CallKeyframeKeeper_converter,
            "CallRender3D": CallRender3D_converter,
            "FlagCall": FlagCall_converter
        }
        result = []
        for call in self.root.find("view").findall("call"):
            if "GUIView" in call.get("from") or "GUIView" in call.get("to"):
                continue

            call_converter = default_converter
            if call.get("class") in mappings:
                call_converter = mappings[call.get("class")]
            result.append(call_converter(call))
        self.calls = result
