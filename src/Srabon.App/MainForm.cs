using System.Drawing.Text;
using System.Runtime.InteropServices;
using Srabon.Converter;

namespace Srabon.App;

internal sealed class MainForm : Form
{
    private static readonly Color Canvas = Color.FromArgb(246, 248, 248);
    private static readonly Color Surface = Color.White;
    private static readonly Color Teal = Color.FromArgb(8, 127, 120);
    private static readonly Color TealDark = Color.FromArgb(5, 103, 98);
    private static readonly Color Ink = Color.FromArgb(28, 39, 39);
    private static readonly Color Muted = Color.FromArgb(91, 105, 104);
    private static readonly Color Border = Color.FromArgb(215, 225, 224);
    private static readonly Color FocusBlue = Color.FromArgb(0, 95, 184);

    private readonly TextBox _romanInput = new();
    private readonly TextBox _banglaOutput = new();
    private readonly ListBox _candidates = new();
    private readonly Label _candidateStatus = new();
    private CandidateSelection _selection = new([string.Empty]);
    private bool _refreshingCandidates;

    public MainForm()
    {
        Text = "Srabon";
        Name = "SrabonMainWindow";
        AccessibleName = "Srabon Bangla phonetic typing converter";
        StartPosition = FormStartPosition.CenterScreen;
        MinimumSize = new Size(720, 690);
        Size = new Size(900, 800);
        BackColor = Canvas;
        ForeColor = Ink;
        Font = new Font("Segoe UI", 10f, FontStyle.Regular, GraphicsUnit.Point);
        AutoScaleMode = AutoScaleMode.Dpi;
        KeyPreview = true;

        BuildLayout();
        SetInput("ami banglay likhi");
        _romanInput.Select(_romanInput.TextLength, 0);
    }

    private void BuildLayout()
    {
        var root = new TableLayoutPanel
        {
            Dock = DockStyle.Fill,
            BackColor = Canvas,
            ColumnCount = 1,
            RowCount = 1,
            Padding = new Padding(28, 20, 28, 18)
        };
        root.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
        Controls.Add(root);

        var content = new TableLayoutPanel
        {
            Dock = DockStyle.Fill,
            ColumnCount = 1,
            RowCount = 10,
            BackColor = Canvas,
            Margin = Padding.Empty
        };
        content.RowStyles.Add(new RowStyle(SizeType.Absolute, 82));
        content.RowStyles.Add(new RowStyle(SizeType.Absolute, 42));
        content.RowStyles.Add(new RowStyle(SizeType.Absolute, 126));
        content.RowStyles.Add(new RowStyle(SizeType.Absolute, 52));
        content.RowStyles.Add(new RowStyle(SizeType.Absolute, 42));
        content.RowStyles.Add(new RowStyle(SizeType.Absolute, 100));
        content.RowStyles.Add(new RowStyle(SizeType.Absolute, 66));
        content.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
        content.RowStyles.Add(new RowStyle(SizeType.Absolute, 48));
        content.RowStyles.Add(new RowStyle(SizeType.Absolute, 38));
        root.Controls.Add(content, 0, 0);

        content.Controls.Add(BuildHeader(), 0, 0);
        content.Controls.Add(BuildSectionLabel("Romanized Bangla input", "TYPE PHONETICALLY"), 0, 1);

        _romanInput.Name = "RomanInput";
        _romanInput.AccessibleName = "Romanized Bangla input";
        _romanInput.AccessibleDescription = "Enter Roman letters to see Bangla conversion candidates.";
        _romanInput.Multiline = true;
        _romanInput.AcceptsReturn = true;
        _romanInput.AcceptsTab = false;
        _romanInput.ScrollBars = ScrollBars.Vertical;
        _romanInput.BorderStyle = BorderStyle.FixedSingle;
        _romanInput.BackColor = Surface;
        _romanInput.ForeColor = Ink;
        _romanInput.Font = new Font("Segoe UI", 16f, FontStyle.Regular, GraphicsUnit.Point);
        _romanInput.Margin = new Padding(0, 0, 0, 10);
        _romanInput.TextChanged += (_, _) => RefreshConversion();
        content.Controls.Add(_romanInput, 0, 2);

        var inputActions = new FlowLayoutPanel
        {
            Dock = DockStyle.Fill,
            FlowDirection = FlowDirection.LeftToRight,
            WrapContents = false,
            BackColor = Canvas,
            Padding = new Padding(0, 4, 0, 0),
            Margin = Padding.Empty
        };
        var sampleButton = CreateButton("Load sample", "Load the exact regression test text", primary: true);
        sampleButton.Click += (_, _) => SetInput("ami banglay likhi");
        var clearButton = CreateButton("Clear input", "Clear Romanized input", primary: false);
        clearButton.Click += (_, _) => SetInput(string.Empty);
        inputActions.Controls.Add(sampleButton);
        inputActions.Controls.Add(clearButton);
        content.Controls.Add(inputActions, 0, 3);

        var outputHeader = BuildSectionLabel("Bangla output", "COMMITTED TEXT PREVIEW");
        content.Controls.Add(outputHeader, 0, 4);

        _banglaOutput.Name = "BanglaOutput";
        _banglaOutput.AccessibleName = "Bangla conversion output";
        _banglaOutput.AccessibleDescription = "The currently selected transliteration candidate.";
        _banglaOutput.Multiline = true;
        _banglaOutput.ReadOnly = true;
        _banglaOutput.BorderStyle = BorderStyle.FixedSingle;
        _banglaOutput.BackColor = Surface;
        _banglaOutput.ForeColor = Ink;
        _banglaOutput.Font = CreateBanglaFont(22f);
        _banglaOutput.ScrollBars = ScrollBars.Vertical;
        _banglaOutput.Margin = new Padding(0, 0, 0, 12);
        content.Controls.Add(_banglaOutput, 0, 5);

        var candidatesHeading = new TableLayoutPanel
        {
            Dock = DockStyle.Fill,
            ColumnCount = 2,
            RowCount = 1,
            BackColor = Canvas,
            Margin = Padding.Empty
        };
        candidatesHeading.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
        candidatesHeading.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
        var candidateTitle = new Label
        {
            Text = "Transliteration candidates",
            Font = new Font("Segoe UI", 12f, FontStyle.Bold),
            ForeColor = Ink,
            TextAlign = ContentAlignment.MiddleLeft,
            Dock = DockStyle.Fill,
            Margin = Padding.Empty
        };
        _candidateStatus.Text = "Alternatives for this input only";
        _candidateStatus.Font = new Font("Segoe UI", 9f, FontStyle.Regular);
        _candidateStatus.ForeColor = Muted;
        _candidateStatus.TextAlign = ContentAlignment.MiddleRight;
        _candidateStatus.Dock = DockStyle.Fill;
        _candidateStatus.AccessibleName = "Candidate count";
        candidatesHeading.Controls.Add(candidateTitle, 0, 0);
        candidatesHeading.Controls.Add(_candidateStatus, 1, 0);
        content.Controls.Add(candidatesHeading, 0, 6);

        _candidates.Name = "CandidateList";
        _candidates.AccessibleName = "Transliteration candidates";
        _candidates.AccessibleDescription = "Use the Up and Down arrow keys to select a candidate.";
        _candidates.Dock = DockStyle.Fill;
        _candidates.IntegralHeight = false;
        _candidates.BorderStyle = BorderStyle.FixedSingle;
        _candidates.BackColor = Surface;
        _candidates.ForeColor = Ink;
        _candidates.Font = CreateBanglaFont(16f);
        _candidates.ItemHeight = Math.Max(_candidates.Font.Height + 10, 36);
        _candidates.DrawMode = DrawMode.OwnerDrawFixed;
        _candidates.DrawItem += DrawCandidate;
        _candidates.SelectedIndexChanged += (_, _) => SelectCandidateFromList();
        content.Controls.Add(_candidates, 0, 7);

        var outputActions = new FlowLayoutPanel
        {
            Dock = DockStyle.Fill,
            FlowDirection = FlowDirection.LeftToRight,
            WrapContents = false,
            BackColor = Canvas,
            Padding = new Padding(0, 6, 0, 0),
            Margin = Padding.Empty
        };
        var copyButton = CreateButton("Copy output", "Copy the selected Bangla output to the clipboard", primary: true);
        copyButton.Click += CopyOutput;
        outputActions.Controls.Add(copyButton);
        content.Controls.Add(outputActions, 0, 8);

        var footnote = new Label
        {
            Text = "Offline prototype · Transliteration only; no predictive suggestions",
            ForeColor = Muted,
            Font = new Font("Segoe UI", 8.5f, FontStyle.Regular),
            Dock = DockStyle.Fill,
            TextAlign = ContentAlignment.MiddleLeft,
            AutoEllipsis = true,
            Margin = Padding.Empty
        };
        content.Controls.Add(footnote, 0, 9);
    }

    private Control BuildHeader()
    {
        var header = new TableLayoutPanel
        {
            Dock = DockStyle.Fill,
            ColumnCount = 1,
            RowCount = 2,
            BackColor = Canvas,
            Margin = Padding.Empty,
            Padding = new Padding(0, 0, 0, 12)
        };
        header.RowStyles.Add(new RowStyle(SizeType.Absolute, 40));
        header.RowStyles.Add(new RowStyle(SizeType.Percent, 100));

        var wordmark = new Label
        {
            Text = "Srabon",
            Font = new Font("Segoe UI", 22f, FontStyle.Bold),
            ForeColor = Ink,
            Dock = DockStyle.Fill,
            TextAlign = ContentAlignment.MiddleLeft,
            Margin = Padding.Empty
        };
        var tagline = new Label
        {
            Text = "Bangla typing, made simple.",
            Font = new Font("Segoe UI", 10.5f, FontStyle.Regular),
            ForeColor = Muted,
            Dock = DockStyle.Fill,
            TextAlign = ContentAlignment.MiddleLeft,
            Margin = Padding.Empty
        };
        header.Controls.Add(wordmark, 0, 0);
        header.Controls.Add(tagline, 0, 1);
        return header;
    }

    private static Control BuildSectionLabel(string title, string caption)
    {
        var panel = new TableLayoutPanel
        {
            Dock = DockStyle.Fill,
            ColumnCount = 2,
            RowCount = 1,
            BackColor = Canvas,
            Margin = Padding.Empty
        };
        panel.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
        panel.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
        panel.Controls.Add(new Label
        {
            Text = title,
            Font = new Font("Segoe UI", 10.5f, FontStyle.Bold),
            ForeColor = Ink,
            Dock = DockStyle.Fill,
            TextAlign = ContentAlignment.MiddleLeft,
            Margin = Padding.Empty
        }, 0, 0);
        panel.Controls.Add(new Label
        {
            Text = caption,
            Font = new Font("Segoe UI", 8f, FontStyle.Bold),
            ForeColor = Muted,
            Dock = DockStyle.Fill,
            TextAlign = ContentAlignment.MiddleRight,
            Margin = Padding.Empty
        }, 1, 0);
        return panel;
    }

    private static Button CreateButton(string text, string accessibleName, bool primary)
    {
        var button = new Button
        {
            Text = text,
            AccessibleName = accessibleName,
            AutoSize = true,
            MinimumSize = new Size(116, 36),
            Padding = new Padding(10, 4, 10, 4),
            Margin = new Padding(0, 0, 10, 0),
            Font = new Font("Segoe UI", 9.5f, FontStyle.Bold),
            UseVisualStyleBackColor = false,
            FlatStyle = FlatStyle.Flat,
            BackColor = primary ? Teal : Surface,
            ForeColor = primary ? Color.White : Ink,
            Cursor = Cursors.Hand
        };
        button.FlatAppearance.BorderColor = primary ? TealDark : Border;
        button.FlatAppearance.BorderSize = 1;
        button.FlatAppearance.MouseOverBackColor = primary ? TealDark : Color.FromArgb(239, 247, 246);
        button.FlatAppearance.MouseDownBackColor = primary ? TealDark : Color.FromArgb(228, 239, 237);
        return button;
    }

    private static Font CreateBanglaFont(float size)
    {
        using var installed = new InstalledFontCollection();
        var names = installed.Families.Select(family => family.Name).ToHashSet(StringComparer.OrdinalIgnoreCase);
        var family = names.Contains("Nirmala UI") ? "Nirmala UI"
            : names.Contains("Vrinda") ? "Vrinda"
            : "Segoe UI";
        return new Font(family, size, FontStyle.Regular, GraphicsUnit.Point);
    }

    private void SetInput(string value)
    {
        _romanInput.Text = value;
        _romanInput.SelectionStart = _romanInput.TextLength;
        _romanInput.SelectionLength = 0;
        _romanInput.Focus();
    }

    private void RefreshConversion()
    {
        var candidates = RomanBanglaConverter.Candidates(_romanInput.Text, maxCandidates: 6);
        _selection = new CandidateSelection(candidates);

        _refreshingCandidates = true;
        _candidates.BeginUpdate();
        try
        {
            _candidates.Items.Clear();
            foreach (var candidate in _selection.Items) _candidates.Items.Add(candidate);
            _candidates.SelectedIndex = 0;
        }
        finally
        {
            _candidates.EndUpdate();
            _refreshingCandidates = false;
        }
        ShowSelectedCandidate();
        _candidateStatus.Text = $"{_selection.Items.Count} {(_selection.Items.Count == 1 ? "candidate" : "candidates")} · same input";
    }

    private void SelectCandidateFromList()
    {
        if (_refreshingCandidates || _candidates.SelectedIndex < 0) return;
        _selection.Select(_candidates.SelectedIndex);
        ShowSelectedCandidate();
    }

    private void ShowSelectedCandidate()
    {
        _banglaOutput.Text = _selection.Selected;
    }

    private void DrawCandidate(object? sender, DrawItemEventArgs e)
    {
        if (e.Index < 0 || e.Index >= _candidates.Items.Count) return;
        var selected = (e.State & DrawItemState.Selected) == DrawItemState.Selected;
        var background = selected ? Color.FromArgb(224, 243, 239) : Surface;
        var foreground = selected ? TealDark : Ink;
        using var backBrush = new SolidBrush(background);
        using var textBrush = new SolidBrush(foreground);
        e.Graphics.FillRectangle(backBrush, e.Bounds);
        var text = _candidates.Items[e.Index]?.ToString() ?? string.Empty;
        var bounds = Rectangle.Inflate(e.Bounds, -12, 0);
        TextRenderer.DrawText(e.Graphics, $"{e.Index + 1}.  {text}", _candidates.Font, bounds, foreground,
            TextFormatFlags.Left | TextFormatFlags.VerticalCenter | TextFormatFlags.NoPrefix | TextFormatFlags.EndEllipsis);
        if ((e.State & DrawItemState.Focus) == DrawItemState.Focus)
            ControlPaint.DrawFocusRectangle(e.Graphics, e.Bounds, foreground, background);
    }

    private void CopyOutput(object? sender, EventArgs e)
    {
        try
        {
            Clipboard.SetText(_selection.Selected);
            MessageBox.Show(this, "Bangla output copied.", "Srabon", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }
        catch (Exception exception) when (exception is ExternalException or ThreadStateException)
        {
            MessageBox.Show(this, "The clipboard is unavailable. Select the output and copy it manually.",
                "Unable to copy", MessageBoxButtons.OK, MessageBoxIcon.Warning);
        }
    }
}
