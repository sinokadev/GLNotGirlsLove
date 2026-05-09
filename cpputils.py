#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse
import re
from pathlib import Path


def read_text(path):
    return Path(path).read_text(encoding="utf-8")


def write_text(path, text):
    Path(path).write_text(text, encoding="utf-8")


# ------------------------------------------------------------
# 1. 특정 줄 범위의 주석 제거
# ------------------------------------------------------------

def remove_comments_from_range(content, start_line, end_line):
    lines = content.splitlines(keepends=True)

    selected = ''.join(lines[start_line - 1:end_line])

    # block comments 제거
    selected = re.sub(
        r'/\*.*?\*/',
        '',
        selected,
        flags=re.DOTALL
    )

    # line comments 제거
    selected = re.sub(
        r'//.*?$',
        '',
        selected,
        flags=re.MULTILINE
    )

    lines[start_line - 1:end_line] = [selected]

    return ''.join(lines)


# ------------------------------------------------------------
# 2. 함수 body 제거
# ------------------------------------------------------------

def remove_function_bodies(text):
    """
    함수 body 제거:
    int foo() { ... }
      -> int foo();

    클래스는 유지됨.
    """

    pattern = re.compile(
        r'''
        (
            [\w:<>\~\*&\s]+?
            \s+
            [\w:~]+
            \s*
            \([^;{}]*\)
            \s*
            (?:const\s*)?
            (?:noexcept\s*)?
        )
        \{
        ''',
        re.VERBOSE
    )

    result = []
    idx = 0

    while True:
        m = pattern.search(text, idx)

        if not m:
            result.append(text[idx:])
            break

        start = m.start()
        brace_start = m.end() - 1

        result.append(text[idx:start])

        depth = 0
        end_pos = brace_start

        for i in range(brace_start, len(text)):
            if text[i] == '{':
                depth += 1
            elif text[i] == '}':
                depth -= 1

                if depth == 0:
                    end_pos = i
                    break

        signature = m.group(1).strip()

        # 함수 선언으로 변경
        result.append(signature + ';')

        idx = end_pos + 1

    return ''.join(result)


# ------------------------------------------------------------
# 3. 헤더가드 자동 생성
# ------------------------------------------------------------

def generate_header_guard(filename):
    name = Path(filename).name.upper()
    name = re.sub(r'[^A-Z0-9]', '_', name)

    return f'{name}'


# ------------------------------------------------------------
# 4. 헤더파일 정리
# ------------------------------------------------------------

def sanitize_header(content, filename):
    body_removed = remove_function_bodies(content)

    guard = generate_header_guard(filename)

    return f'''// AUTO GENERATED HEADER
#ifndef {guard}
#define {guard}

{body_removed.strip()}

#endif // {guard}
'''


# ------------------------------------------------------------
# CLI
# ------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(
        description="C++ Utility Tool"
    )

    sub = parser.add_subparsers(dest="command")

    # --------------------------------------------------------
    # remove-comments
    # --------------------------------------------------------

    p1 = sub.add_parser(
        "remove-comments",
        help="특정 라인 범위의 주석 제거"
    )

    p1.add_argument("input")
    p1.add_argument("output", nargs="?")

    p1.add_argument(
        "--start",
        type=int,
        required=True
    )

    p1.add_argument(
        "--end",
        type=int,
        required=True
    )

    # --------------------------------------------------------
    # sanitize-header
    # --------------------------------------------------------

    p2 = sub.add_parser(
        "sanitize-header",
        help="헤더파일 자동 정리"
    )

    p2.add_argument("input")
    p2.add_argument("output", nargs="?")

    # --------------------------------------------------------

    args = parser.parse_args()

    # --------------------------------------------------------
    # remove-comments
    # --------------------------------------------------------

    if args.command == "remove-comments":

        content = read_text(args.input)

        result = remove_comments_from_range(
            content,
            args.start,
            args.end
        )

        output_path = args.output if args.output else args.input

        write_text(output_path, result)

        print("주석 제거 완료")
        print(f"출력 파일: {output_path}")

    # --------------------------------------------------------
    # sanitize-header
    # --------------------------------------------------------

    elif args.command == "sanitize-header":

        content = read_text(args.input)

        result = sanitize_header(
            content,
            args.input
        )

        output_path = args.output if args.output else args.input

        write_text(output_path, result)

        print("헤더 정리 완료")
        print(f"출력 파일: {output_path}")

    else:
        parser.print_help()


if __name__ == "__main__":
    main()