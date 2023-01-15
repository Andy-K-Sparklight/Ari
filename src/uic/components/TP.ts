/// <reference path="types.d.ts" />
import { useRef, useState } from "react";
import zhCN from "../i18n/zh-CN.yaml";

const LANGCTL: Record<string, any> = {
  "zh-CN": zhCN,
};

let SELECTED_LANG = "zh-CN";

function findVal(s: string): string[] {
  const keys = s.split(".");
  let cur = LANGCTL[SELECTED_LANG];
  for (const k of keys) {
    cur = cur[k];
    if (cur === undefined) {
      return [];
    }
  }
  return cur;
}

export function tr(k?: string, vars: Record<string, string> = {}): string {
  if (k === undefined) {
    return "";
  }
  if (k.startsWith("!")) {
    // Means no translation
    return k.slice(1);
  }
  const vals = findVal(k);
  let s: string;
  if (vals instanceof Array) {
    s = vals[Math.floor(Math.random() * vals.length)] || "";
  } else {
    s = String(vals);
  }
  for (const [x, v] of Object.entries(vars)) {
    console.log(x + " -> " + v);
    s = s.replace(new RegExp("\\{" + x + "\\}", "g"), v);
  }
  return s;
}

export function useTr(k: string): [string, () => void] {
  const vals = findVal(k);
  const [ind, setInd] = useState(0);
  const updator = useRef(() => {});
  updator.current = () => {
    let ni = ind + 1;
    if (ni >= vals.length) {
      ni = 0;
    }
    setInd(ni);
  };

  return [vals[ind], updator.current];
}
