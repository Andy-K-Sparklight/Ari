import React from "react";
import { tr } from "./TP";

interface SubmitProps {
  onClick?: () => void;
  ok?: boolean;
  count?: number;
}

export function Submit(props: SubmitProps): JSX.Element {
  return (
    <div
      className={"a2submit"}
      onClick={() => {
        if (props.onClick) {
          props.onClick();
        }
      }}
      style={{
        color: props.ok ? "var(--a2-base)" : "var(--a2-warn)",
      }}
    >
      {props.count !== undefined
        ? tr("Commons.SubmitCount", { Count: props.count?.toString() || "0" })
        : tr(props.ok ? "Commons.Submit" : "Commons.SubmitFail")}
    </div>
  );
}
