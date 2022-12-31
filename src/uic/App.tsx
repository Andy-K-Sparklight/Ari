import { createRoot } from "react-dom/client";
import React from "react";
import { initProtocol, sendMessage } from "./Protocol";

function App() {
  return (
    <>
      <style>
        {`#contentl {
            position: fixed;
            z-index: -1;
            width: 70vw;
            height: 100vh;
            left: 0;
            top: 0;
            transform: translateX(-20vw);
        }
        #contentl::after {
            content: '';
            position: absolute;
            z-index: -2;
            left: 0;
            top: 0;
            right: 0;
            bottom: 0;
            transform: skewX(-15deg);
            box-shadow: 5px 0px 0px #24bfa550;
            background-color: #24bfa5;
        }
        #mbox {
            position: fixed;
            left: 5vw;
        }
        #mtitle {
            width: 60vw;
            font-size: 2rem;
            color: white;
        }
        #mdesc {
            left: 5vw;
            width: 40vw;
            font-size: 1rem;
            color: #a1ffee;
        }
        #mprogo {
            width: 40vw;
            border: 2px white solid;
            padding: 3px;
            background-color: #24bfa5;
            height: 1rem;
        }
        #mprogi {
            height: 100%;
            width: 60%;
            background-color: white;
        }
    `}
      </style>
      <div id={"contentl"}></div>
      <div id={"mbox"}>
        <h1 id={"mtitle"}>Please Wait...</h1>
        <p id={"mdesc"}>We're preparing your game. Please be patient.</p>
        <div id={"mprogo"}>
          <div id={"mprogi"}></div>
        </div>
        <p id={"mdesc"}>
          60% Compleled
          <br />
          Now downloading assets
          <br />
          2076/3460 Files OK
        </p>
      </div>
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
}

(() => {
  console.log("Hi.");
  console.log("It's so good to see you again.");
  console.log("I missed you.");
  void main();
})();
