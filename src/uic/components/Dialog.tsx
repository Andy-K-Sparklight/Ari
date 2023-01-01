import React from "react";
import { DisPanel } from "./DisPanel";
import { DisTitle } from "./DisTitle";
import { Layer } from "./Layer";
import { Mask } from "./Mask";
import { OpPanel } from "./OpPanel";

interface DialogProps {
  title?: string;
  desc?: React.ReactNode;
  children?: React.ReactNode;
  show?: boolean;
}

export function Dialog(props: DialogProps): JSX.Element {
  return (
    <>
      <Layer />
      <Mask color={"orange"} />
      <DisPanel show={props.show}>
        <DisTitle>{props.title}</DisTitle>
        {props.desc}
      </DisPanel>
      <OpPanel show={props.show}>{props.children}</OpPanel>
    </>
  );
}
