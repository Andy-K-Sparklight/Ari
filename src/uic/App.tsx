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
  useEffect(() => {
    window.addEventListener("UIDraw", (e) => {
      if (e instanceof CustomEvent) {
        const ins = e.detail;
        setDrawInstr(ins);
        setSubmitLock(false);
      }
    });
  }, []);
  const obj = JSON.parse(drawInstr) as DrawInstr;
  console.log(obj);
  if (Object.keys(obj).length == 0) {
    return <></>;
  }
  return (
    <>
      <Layer />
      <Mask color={"orange"} />
      <DisPanel show={!submitLock}>
        {obj.widgets.map((w, i) => {
          if (w.variant == "Title") {
            return <DisTitle key={i}>{tr(w.props["Msg"])}</DisTitle>;
          } else if (w.variant == "Text") {
            return <UIText key={i}>{tr(w.props["Msg"])}</UIText>;
          }
        })}
      </DisPanel>
      <OpPanel show={!submitLock}>
        {obj.entries.map((e, i) => {
          if (e.variant == "Choice" || e.variant == "") {
            return (
              <Button
                key={i}
                text={tr(e.props["Label"])}
                hint={e.props["Hint"]}
                warn={e.props["Warn"]}
                img={e.props["Img"]}
                onClick={() => {
                  if (!submitLock) {
                    setSubmitLock(true);
                    sendMessage(
                      "UIDrawOK",
                      JSON.stringify({ userChoice: e.jmpLabel })
                    );
                  }
                }}
              />
            );
          }
        })}
      </OpPanel>
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
