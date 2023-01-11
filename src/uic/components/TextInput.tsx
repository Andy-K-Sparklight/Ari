import React from "react";
import { Tag } from "./Tag";

interface TextInputProps {
  tag?: string;
  placeholder?: string;
  value?: string;
  onChange?: (s: string) => void;
  type?: string;
}

export function TextInput(props: TextInputProps): JSX.Element {
  return (
    <>
      <Tag s={props.tag} />
      <input
        className={"a2input"}
        type={props.type}
        value={props.value}
        onChange={(s) => {
          if (props.onChange) {
            props.onChange(s.target.value);
          }
        }}
        placeholder={props.placeholder}
      />
    </>
  );
}
