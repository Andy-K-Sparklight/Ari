import { createRoot } from "react-dom/client";
import React, { useEffect, useState } from "react";
import { initProtocol, sendMessage } from "./Protocol";
import "./css/Essential.css";
import "./css/Revive.css";
import { Layer } from "./components/Layer";
import { DisPanel } from "./components/DisPanel";
import { OpPanel } from "./components/OpPanel";
import { DisTitle } from "./components/DisTitle";
import { UIText } from "./components/UIText";
import { Button } from "./components/Button";
import { tr } from "./components/TP";
import { Mask } from "./components/Mask";
import { Progress } from "./components/Progress";
import { IMAGES } from "./components/ImgSrc";
import { NetSpeed } from "./components/NetSpeed";
import { Logs } from "./components/Logs";
import { TextInput } from "./components/TextInput";
import { Submit } from "./components/Submit";

interface DrawInstr {
  widgets: { variant: string; props: Record<string, string> }[];
  entries: {
    variant: string;
    jmpLabel: string;
    props: Record<string, string>;
  }[];
}

function App() {
  const [drawInstr, setDrawInstr] = useState("{}");
  const [submitLock, setSubmitLock] = useState(false);
  const [dataStack, setDataStack] = useState<string[]>([]);
  const [dataErrStack, setDataErrStack] = useState<boolean[]>([true]);
  useEffect(() => {
    window.addEventListener("UIDraw", (e) => {
      if (e instanceof CustomEvent) {
        const ins = e.detail;
        const draw = () => {
          setDrawInstr(ins);
          setSubmitLock(false);
          window.dispatchEvent(new CustomEvent("HTTPReq", { detail: "" })); // Reset host
          setTimeout(() => {
            sendMessage("UIDrew", "");
          }, 200);
        };
        if (!submitLock) {
          setSubmitLock(true);
          setTimeout(() => {
            draw();
          }, 200);
        } else {
          draw();
        }
      }
    });
  }, []);
  const obj = JSON.parse(drawInstr) as DrawInstr;
  if (Object.keys(obj).length == 0) {
    return <></>;
  }
  let ok = true;
  for (const p of dataErrStack) {
    if (p) {
      ok = false;
      break;
    }
  }
  return (
    <>
      <Layer />
      <Logs />
      <Mask color={"orange"} />
      <DisPanel show={!submitLock}>
        {obj.widgets.map((w, i) => {
          const def = w.props["Argv0"];
          if (w.variant == "Title") {
            return (
              <DisTitle key={i}>{tr(def || w.props["Msg"], w.props)}</DisTitle>
            );
          } else if (w.variant == "Text") {
            return <UIText key={i} text={tr(def || w.props["Msg"], w.props)} />;
          } else if (w.variant == "Icon") {
            return (
              <div
                key={i}
                className={"a2img a2glowing"}
                style={{ width: "4rem", height: "4rem" }}
              >
                {IMAGES[def || w.props["Icon"]]}
              </div>
            );
          } else if (w.variant == "Img") {
            return (
              <React.Fragment key={i}>
                {IMAGES[def || w.props["Image"]]}
                <br />
                <br />
              </React.Fragment>
            );
          }
        })}
      </DisPanel>
      <OpPanel show={!submitLock}>
        {obj.entries.map((e, i) => {
          if (e.variant == "Choice" || e.variant == "") {
            return (
              <Button
                key={i}
                tag={tr(e.props["Tag"], e.props)}
                text={tr(e.props["Label"], e.props)}
                hint={e.props["Hint"]}
                warn={tr(e.props["Warn"], e.props)}
                img={e.props["Icon"]}
                onClick={() => {
                  if (!submitLock) {
                    setSubmitLock(true);
                    setTimeout(() => {
                      sendMessage(
                        "UIResponse",
                        JSON.stringify({ userChoice: e.jmpLabel })
                      );
                    }, 200);
                  }
                }}
              />
            );
          } else if (e.variant == "Progress") {
            return <Progress key={i} />;
          } else if (e.variant == "Input") {
            return (
              <TextInput
                key={i}
                tag={tr(e.props["Tag"], e.props)}
                value={dataStack[i] || ""}
                type={e.props["Type"]}
                onChange={(s, e) => {
                  const p = dataStack.concat();
                  p[i] = s;
                  setDataStack(p);

                  const ep = dataErrStack.concat();
                  ep[i] = e;
                  setDataErrStack(ep);
                }}
                placeholder={tr(e.props["Ph"], e.props)}
              />
            );
          } else if (e.variant == "Submit") {
            return (
              <Submit
                ok={ok}
                onClick={() => {
                  if (!ok) {
                    return;
                  }
                  if (!submitLock) {
                    setSubmitLock(true);
                    setDataStack([]);
                    setDataErrStack([true]);
                    setTimeout(() => {
                      sendMessage(
                        "UIResponse",
                        JSON.stringify({ userInput: dataStack })
                      );
                    }, 200);
                  }
                }}
              />
            );
          }
        })}
      </OpPanel>
      <NetSpeed />
    </>
  );
}

async function main() {
  // Init
  initProtocol();
  console.log("Checking protocol.");
  console.log('"What makes you fly is not the wind..."');
  console.log(await sendMessage("Ping", ""));

  // Render
  const ele = document.getElementById("a2root");
  if (ele) {
    const root = createRoot(ele);
    root.render(<App />);
  }

  void sendMessage("Ready", "");
}

(() => {
  console.log("Hi.");
  console.log("It's so good to see you again.");
  console.log("I missed you.");
  void main();
})();
