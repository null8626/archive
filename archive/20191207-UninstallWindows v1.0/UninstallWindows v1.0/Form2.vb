Public Class Form2
    Private Sub DomainUpDown1_SelectedItemChanged(sender As Object, e As EventArgs)
        Button2.Enabled = True
    End Sub

    Private Sub CheckedListBox1_SelectedIndexChanged(sender As Object, e As EventArgs)

    End Sub
    Private Sub Button2_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button2.Click
        Dim oForm As New Form3()

        Me.Hide()
        oForm.ShowDialog()

        Me.Close()
    End Sub

    Private Sub Form2_Load(sender As Object, e As EventArgs) Handles MyBase.Load

    End Sub
    Private Sub Button1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button1.Click
        Dim oForm As New Form4()

        Me.Hide()
        oForm.ShowDialog()

        Me.Close()
    End Sub
End Class