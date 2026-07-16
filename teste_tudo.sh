#!/bin/bash

EXEC_CPP="./filtromediana"
CPP_FILE="filtromediana.cpp"
MD_FILE="relatorio_comparacao.md"
DIRETORIO_ARQUIVOS="./arquivos"

echo "========================================"
echo "Iniciando comparação PBM vs PDF/ODT"
echo "========================================"
echo "Arquivo Sendo Processado: $CPP_FILE"

# 1. Verificação de Dependências
if ! command -v odt2txt &> /dev/null || ! command -v pdftotext &> /dev/null || ! command -v unzip &> /dev/null; then
    echo "Erro: Dependências ausentes (odt2txt, pdftotext ou unzip)."
    exit 1
fi

# 2. Compilação Segura
echo "Compilando o código C++..."
if ! g++ "$CPP_FILE" -o filtromediana; then
    echo "Erro: Falha na compilação. Interrompendo execução."
    exit 1
fi

# 3. Inicialização do Arquivo Markdown
echo "# Relatório de Comparação: Extrator C++ vs Gabarito" > "$MD_FILE"
echo "Gerado em: $(date '+%Y-%m-%d %H:%M:%S')" >> "$MD_FILE"
echo "" >> "$MD_FILE"
echo "| Arquivo | Letras (C++ / Gab) | Palavras (C++ / Gab) | Linhas (C++ / Gab) | Colunas (C++ / Gab) | Status Global |" >> "$MD_FILE"
echo "| :--- | :---: | :---: | :---: | :---: | :---: |" >> "$MD_FILE"

for pbm_file in $DIRETORIO_ARQUIVOS/*.pbm; do
    [ -e "$pbm_file" ] || continue

    base_name=$(basename "$pbm_file" .pbm)
    dir_name=$(dirname "$pbm_file")
    
    odt_file="${dir_name}/${base_name}.odt"
    pdf_file="${dir_name}/${base_name}.pdf"

    echo "Processando: $base_name"

    if [ ! -f "$odt_file" ] || [ ! -f "$pdf_file" ]; then
        echo "-> Ignorado: ODT ou PDF correspondente ausente."
        echo "----------------------------------------"
        continue
    fi

    # 4. Extração do C++
    cpp_output=$("$EXEC_CPP" "$pbm_file" "./saida1.pbm" "./saida1_colorido.ppm")
    
    cpp_let=$(echo "$cpp_output" | grep -i "Letras:"   | grep -oE '[0-9]+' || echo "0")
    cpp_pal=$(echo "$cpp_output" | grep -i "Palavras:" | grep -oE '[0-9]+' || echo "0")
    cpp_lin=$(echo "$cpp_output" | grep -i "Linhas:"   | grep -oE '[0-9]+' || echo "0")
    cpp_col=$(echo "$cpp_output" | grep -i "Colunas:"  | grep -oE '[0-9]+' || echo "0")

    # 5. Extração do Gabarito Real
    gab_let=$(odt2txt "$odt_file" | tr -d "[:space:]" | wc -m)
    gab_pal=$(odt2txt "$odt_file" | wc -w)
    
    # ALTERAÇÃO AQUI: Inclusão da flag -layout para agrupar colunas na mesma linha horizontal
    gab_lin=$(pdftotext -layout "$pdf_file" - | grep -c '[^[:space:]]')
    
    gab_col=$(unzip -p "$odt_file" styles.xml content.xml 2>/dev/null | grep -o 'fo:column-count="[0-9]*"' | head -n 1 | grep -oE '[0-9]+')
    [ -z "$gab_col" ] && gab_col=1

    # 6. Lógica de Marcadores (Terminal e Markdown)
    marca_let="[ ERRO ]"
    md_let="❌"
    if [ "$cpp_let" -eq "$gab_let" ]; then
        marca_let="[  OK  ]"
        md_let="✅"
    fi

    marca_pal="[ ERRO ]"
    md_pal="❌"
    if [ "$cpp_pal" -eq "$gab_pal" ]; then
        marca_pal="[  OK  ]"
        md_pal="✅"
    fi

    marca_lin="[ ERRO ]"
    md_lin="❌"
    if [ "$cpp_lin" -eq "$gab_lin" ]; then
        marca_lin="[  OK  ]"
        md_lin="✅"
    fi

    marca_col="[ ERRO ]"
    md_col="❌"
    if [ "$cpp_col" -eq "$gab_col" ]; then
        marca_col="[  OK  ]"
        md_col="✅"
    fi

    # 7. Formatação de Saída no Terminal
    echo -e "\t\tC++\tGabarito\tStatus"
    # echo -e "Letras:  \t$cpp_let\t$gab_let\t\t$marca_let"
    echo -e "Palavras:\t$cpp_pal\t$gab_pal\t\t$marca_pal"
    echo -e "Linhas:  \t$cpp_lin\t$gab_lin\t\t$marca_lin"
    echo -e "Colunas:\t$cpp_col\t$gab_col\t\t$marca_col"

    # 8. Status Global e Gravação
    status_terminal="DIVERGÊNCIA"
    status_md="🔴 Reprovado"
    if [ "$cpp_lin" -eq "$gab_lin" ] && [ "$cpp_pal" -eq "$gab_pal" ] && [ "$cpp_col" -eq "$gab_col" ]; then
    # if [ "$cpp_lin" -eq "$gab_lin" ] && [ "$cpp_pal" -eq "$gab_pal" ] && [ "$cpp_col" -eq "$gab_col" ] && [ "$cpp_let" -eq "$gab_let" ]; then
        status_terminal="APROVADO"
        status_md="🟢 Aprovado"
    fi
    echo "-> RESULTADO: $status_terminal"
    echo "----------------------------------------"

    # 9. Inserção da Linha no Arquivo Markdown
    echo "| \`$base_name\` | $cpp_let / $gab_let $md_let | $cpp_pal / $gab_pal $md_pal | $cpp_lin / $gab_lin $md_lin | $cpp_col / $gab_col $md_col | **$status_md** |" >> "$MD_FILE"
    sleep 2
done

echo "Concluído. Relatório salvo em: ./$MD_FILE"
