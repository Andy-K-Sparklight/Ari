import { createRoot } from "react-dom/client";
import React from "react";
import { initProtocol, sendMessage } from "./Protocol";
import "./css/Essential.css";
import "./css/Revive.css";

function App() {
  return <></>;
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
