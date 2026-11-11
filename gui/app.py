
import json
import subprocess
from pathlib import Path

import streamlit as st


PROJECT_ROOT = Path(__file__).resolve().parent.parent
BINARY_CANDIDATES = [
    Path('/tmp/filesearch_berkay_v2'),
    PROJECT_ROOT / 'build' / 'filesearch',
]

st.set_page_config(page_title='FileSearchManager V2', page_icon='🔎', layout='wide')
st.title('🔎 FileSearchManager V2')
st.caption('Berkay: arayuz, dosya filtreleme ve C++ arama motoru entegrasyonu')

with st.sidebar:
    st.header('Arama ayarlari')
    folder = st.text_input('Klasor yolu (Ubuntu/WSL)', value='samples')
    query = st.text_input('Aranacak kelime', value='memory')
    threads = st.slider('Thread sayisi', min_value=1, max_value=8, value=2)

    st.subheader('Dosya filtreleri')
    extensions = st.multiselect(
        'Dosya uzantilari',
        ['.txt', '.csv', '.md', '.log', '.json'],
        default=['.txt'],
    )
    recursive = st.checkbox('Alt klasorleri de tara', value=False)
    filename_filter = st.text_input('Dosya adinda gecen ifade', value='')
    min_kb = st.number_input('Minimum dosya boyutu (KB)', min_value=0, value=0, step=1)
    max_kb = st.number_input('Maksimum dosya boyutu (KB, 0 = sinirsiz)', min_value=0, value=0, step=1)

folder_path = Path(folder).expanduser()
if not folder_path.is_absolute():
    folder_path = PROJECT_ROOT / folder_path

choose_files = st.checkbox('Yalnizca sectigim dosyalarda ara', value=False)
selected_files = []
if choose_files and folder_path.is_dir():
    try:
        paths = folder_path.rglob('*') if recursive else folder_path.iterdir()
        available_files = sorted(
            path.relative_to(folder_path).as_posix()
            for path in paths
            if path.is_file()
        )
        selected_files = st.multiselect('Aranacak dosyalari sec', available_files)
    except OSError as exc:
        st.error(f'Dosyalar listelenemedi: {exc}')

if not folder_path.is_dir():
    st.warning('Klasor bulunamadi. Ubuntu/WSL icin gecerli bir yol gir.')

if st.button('Dosyalarda ara', type='primary', use_container_width=True):
    binary = next((path for path in BINARY_CANDIDATES if path.is_file()), None)

    if not query:
        st.error('Aranacak kelime bos olamaz.')
    elif not extensions:
        st.error('En az bir dosya uzantisi sec.')
    elif not folder_path.is_dir():
        st.error('Gecerli bir klasor belirt.')
    elif max_kb and min_kb > max_kb:
        st.error('Minimum boyut, maksimum boyuttan buyuk olamaz.')
    elif choose_files and not selected_files:
        st.error('Dosya secimi aciksa en az bir dosya secmelisin.')
    elif binary is None:
        st.error('Derlenmis C++ programi bulunamadi. Once C++ programini derle.')
        st.code(
            'g++ -std=c++17 -Wall -Wextra -Wpedantic -pthread -Iinclude '
            'src/main.cpp src/file_scanner.cpp src/search.cpp src/parallel_search.cpp '
            '-o /tmp/filesearch_berkay_v2',
            language='bash',
        )
    else:
        command = [
            str(binary), folder, query, str(threads),
            '--json', '--extensions', ','.join(extensions),
        ]
        if recursive:
            command.append('--recursive')
        if filename_filter:
            command += ['--filename', filename_filter]
        if min_kb:
            command += ['--min-kb', str(min_kb)]
        if max_kb:
            command += ['--max-kb', str(max_kb)]
        for filename in selected_files:
            command += ['--file', filename]

        try:
            process = subprocess.run(
                command,
                cwd=PROJECT_ROOT,
                capture_output=True,
                text=True,
                encoding='utf-8',
                errors='replace',
                timeout=120,
                check=False,
            )
            result = json.loads(process.stdout)
            st.session_state['result'] = result
            st.session_state['last_query'] = query
            st.session_state['exit_code'] = process.returncode
            st.session_state['stderr'] = process.stderr
        except subprocess.TimeoutExpired:
            st.error('Arama 120 saniye icinde tamamlanamadi.')
        except (OSError, ValueError) as exc:
            st.error(f'C++ programi calistirilamadi veya JSON okunamadi: {exc}')
            if 'process' in locals() and process.stdout:
                st.code(process.stdout[:2000], language='text')

if 'result' in st.session_state:
    result = st.session_state['result']
    st.subheader('Arama sonuclari')
    st.caption(f'Son arama: {st.session_state["last_query"]}')

    col1, col2, col3, col4 = st.columns(4)
    col1.metric('Taranan dosya', result.get('files_scanned', 0))
    col2.metric('Eslesen satir', result.get('matching_lines', 0))
    col3.metric('Thread', result.get('threads', 0))
    col4.metric('Toplam sure', f'{result.get("total_ms", 0):.3f} ms')

    st.caption(
        f'Dosya tarama: {result.get("scan_ms", 0):.3f} ms | '
        f'Paralel arama: {result.get("search_ms", 0):.3f} ms'
    )

    errors = result.get('errors', [])
    for error in errors:
        st.error(error)
    if st.session_state.get('stderr'):
        st.warning(st.session_state['stderr'])
    if st.session_state.get('exit_code') not in (0, 1):
        st.warning(f'C++ programi cikis kodu: {st.session_state["exit_code"]}')

    matches = result.get('matches', [])
    if matches:
        rows = [
            {
                'Dosya': match.get('file_path', ''),
                'Satir': match.get('line_number', ''),
                'Icerik': match.get('line_text', ''),
            }
            for match in matches
        ]
        st.dataframe(rows, use_container_width=True, hide_index=True)
    else:
        st.info('Eslesen satir bulunamadi.')

    st.download_button(
        'JSON sonucunu indir',
        data=json.dumps(result, ensure_ascii=False, indent=2),
        file_name='filesearch_results.json',
        mime='application/json',
    )
