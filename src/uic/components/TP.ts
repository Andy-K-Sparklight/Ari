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

export function tr(k?: string): string {
  if (k === undefined) {
    return "";
  }
  const vals = findVal(k);
  if (vals instanceof Array) {
    return vals[Math.floor(Math.random() * vals.length)] || "";
  } else {
    return String(vals);
  }
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
